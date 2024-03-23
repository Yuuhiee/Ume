#include "umepch.h"
#include "Scene.h"
#include "Entity.h"
#include "Component.h"
#include "Ume/Renderer/Renderer2D.h"
#include "glm/gtx/euler_angles.hpp"
#include <stack>

namespace Ume
{
	Entity ECS::CreateEntity(const std::string& name/* = ""*/)
	{
		Entity entity = { m_Registry.create(), this };

		static uint32_t index = 0;
		std::string tag = name;
		if (tag.empty())
		{
			tag = "Entity_" + std::to_string(index);
			index++;
		}
		entity.AddComponent<TagComponent>(tag);
		entity.AddComponent<Transform>();
		return entity;
	}

	void ECS::OnUpdate(Timestep ts)
	{
		CameraBase* mainCamera = nullptr;
		Transform* cameraTransform = nullptr;

		{
			auto group = m_Registry.view<CameraComponent, Transform>();

			for (auto& entity : group)
			{
				auto& [camera, transform] = group.get<CameraComponent, Transform>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = &transform;
					break;
				}
			}
		}

		if (mainCamera)
		{
			//auto rotation = glm::toMat4(glm::quat(cameraTransform->EulerRotation));
			auto rotation = glm::yawPitchRoll(
				glm::radians(cameraTransform->EulerRotation.y),
				glm::radians(cameraTransform->EulerRotation.x),
				glm::radians(cameraTransform->EulerRotation.z)
			);
			auto translation = glm::translate(glm::mat4(1.0f), -cameraTransform->Position);
			//glm::mat4 viewProjection = mainCamera->GetProjection() * glm::inverse(*cameraTransform);
			glm::mat4 viewProjection = mainCamera->GetProjection() * translation * rotation;
			Renderer2D::BeginScene(viewProjection);
			{
				auto group = m_Registry.view<SpriteRenderComponent, Transform>();

				for (auto& entity : group)
				{
					auto& [sprite, transform] = group.get<SpriteRenderComponent, Transform>(entity);
					Renderer2D::DrawQuad(transform.ModelMatrix, sprite.Color);
				}
			}
			Renderer2D::EndScene();
		}
	}

	/////////////
	static uint32_t StartID = 1;

	HitPayload Scene::TraceRay(const Ray& ray)
	{
		return m_BVHRoot->Intersect(ray);
	}

	HitPayload Scene::SampleLight(float& pdf)
	{
		float areaSum = 0.0f;
		for (const auto& light : Lights)
			areaSum += light->Area;

		float p = Random::Float() * areaSum;
		areaSum = 0.0f;
		for (const auto& light : Lights)
		{
			areaSum += light->Area;
			if (p <= areaSum)
				return light->Sample(pdf);
		}
		return {};
	}

	glm::vec3 Scene::GetEnvironment(const glm::vec3& direction)
	{
		if (m_EnvTexture) return m_EnvTexture->Sample(direction) * Config.Ambient;
		auto color = direction * 0.5f + 0.5f;
		return color * Config.Ambient;
	}

	Ume::Object* Scene::SelectObject(const glm::ivec2& clickPos)
	{
		auto camera = (RTCamera*)(ActivedCamera.get());
		Ray ray = { camera->Position, camera->GetRayDirection(clickPos.x, camera->GetHeight() - 1 - clickPos.y) };
		HitPayload payload = TraceRay(ray);

		SelectedObject = payload.Hit ? payload.Object : nullptr;
		return SelectedObject;
	}

	void Scene::BuildBVH()
	{
		Lights.clear();

		bool needRebuild = false;
		for (auto& object : Objects)
		{
			needRebuild |= object->Build();
			if (object->Emissive())
				Lights.push_back(object);
		}
		if (needRebuild)
			m_BVHRoot = BVHManager::RecursiveBuild(Objects);
		UME_INFO("Build");
	}

	void Scene::AddObject(const Ref<Ume::Object>& object)
	{
		Objects.push_back(object);
		object->ID = StartID++;
	}

	void Scene::DelObject(uint32_t id)
	{
		if (SelectedObject && SelectedObject->ID == id) SelectedObject = nullptr;

		auto condition = [id](const auto& light) { return light->ID == id; };

		auto lights_it = std::find_if(Lights.begin(), Lights.end(), condition);
		if (lights_it != Lights.end())
			Lights.erase(lights_it);

		auto objects_it = std::find_if(Objects.begin(), Objects.end(), condition);
		if (objects_it != Objects.end())
			Objects.erase(objects_it);
		UME_INFO("Delete: {}", id);
	}
}