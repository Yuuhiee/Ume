#pragma once
#include "entt.hpp"
#include "Ume/Core/Timestep.h"
#include "glm/glm.hpp"
#include "Component.h"
#include "Ume/Util/Random.h"
#include "Ume/Renderer/RayTracing/Ray.h"
#include "Ume/Renderer/RayTracing/RTCamera.h"
#include "Ume/Renderer/RayTracing/Object.h"
#include "Ume/Renderer/RayTracing/BVH.h"
#include "Ume/Renderer/Texture.h"

namespace Ume
{
	class Entity;

	class ECS
	{
		friend class Entity;
	public:
		ECS() = default;

		Entity CreateEntity(const std::string& name = "");

		void OnUpdate(Timestep ts);

		inline entt::registry& Reg() { return m_Registry; }
	private:
		entt::registry m_Registry;
	};

	////////////////////
	////////////////////
	////////////////////

	struct Light
	{
		Transform* ModelMatrix;
		Emission* Emission;
		SphereComponent* Sphere;

		float SurfaceArea() const
		{
			if (Sphere->Radius)
				return FOUR_PI * Sphere->Radius * Sphere->Radius;
			return 0.0f;
		}

		glm::vec3 SampleSurface(float& pdf) const
		{
			if (Sphere->Radius)
			{
				auto dir = Random::Sphere();
				auto sample = Sphere->Center + Sphere->Radius * dir;
				pdf = 1.0f / SurfaceArea();
				return sample;
			}
			pdf = 1.0f;
			return Sphere->Center;
		}


	};

	class Scene
	{
	public:
		struct Configuration
		{
			float Ambient = 0.3f;
		};
		Configuration Config;
		Scene() = default;
		std::vector<Ref<Camera>> Cameras;
		Ref<Camera> ActivedCamera;
		std::vector<Ref<Ume::Object>> Objects;
		std::vector<Ref<Ume::Object>> Lights;
		std::vector<std::array<glm::mat4, 6>> ShadowMatrices;
		Ume::Object* SelectedObject = nullptr;

		HitPayload TraceRay(const Ray& ray);
		HitPayload SampleLight(float& pdf);
		glm::vec3 GetEnvironment(const glm::vec3& direction);
		Ume::Object* SelectObject(const glm::ivec2& clickPos);
        inline Ref<Texture2D>& GetEnvTexture() { return m_EnvTexture; }

		void BuildBVH();
		void AddObject(const Ref<Ume::Object>& object);
		void DelObject(uint32_t id);
		inline void SetEnvTexture(const Ref<Texture2D>& texture) { m_EnvTexture = texture; }
	private:
		Ref<BVHBuildNode> m_BVHRoot;
		Ref<Texture2D> m_EnvTexture = nullptr;

		void RebuildLight();
	};
}