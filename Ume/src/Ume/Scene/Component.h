#pragma once

#include "glm/glm.hpp"
#include "Ume/Renderer/Camera.h"
#include "Ume/Renderer/RayTracing/Material.h"

#define DEFAULT_CONSTRUCTORS(className) \
	className() = default;\
	className(const className&) = default;\
	\

namespace Ume
{
	struct Transform
	{
		glm::mat4 ModelMatrix = glm::mat4(1.0f);
		glm::mat3 NormalMatrix = glm::mat3(1.0f);
		glm::vec3 Position = glm::vec3(0.0f);
		glm::vec3 EulerRotation = glm::vec3(0.0f);
		glm::vec3 Scale = glm::vec3(1.0f);

		DEFAULT_CONSTRUCTORS(Transform)
		Transform(const glm::mat4& transform) : ModelMatrix(transform) {}
		Transform(const glm::vec3& position, const glm::vec3& scale = glm::vec3(1.0f), const glm::vec3& rotation = glm::vec3(0.0f))
			: Position(position), EulerRotation(rotation), Scale(scale)
			{
				UpdateTransform();
			}


		void UpdateTransform();

		void SetPosition(const glm::vec3& position) { Position = position; UpdateTransform(); }
		void SetRotation(const glm::vec3& rotation) { EulerRotation = rotation; UpdateTransform(); }
		void SetScale(const glm::vec3& scale) { Scale = scale; UpdateTransform(); }
	};

	struct TagComponent
	{
		std::string Tag;

		DEFAULT_CONSTRUCTORS(TagComponent)
		TagComponent(const std::string& tag) : Tag(tag) {}

		operator std::string& () { return Tag; }
		operator const std::string& () const { return Tag; }
	};

	struct SpriteRenderComponent
	{
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };

		DEFAULT_CONSTRUCTORS(SpriteRenderComponent)
		SpriteRenderComponent(const glm::vec4& color) : Color(color) {}
	};

	struct CameraComponent
	{
		CameraBase Camera;
		bool Primary;

		DEFAULT_CONSTRUCTORS(CameraComponent)
		CameraComponent(const glm::mat4& projection, bool primary = false) 
			: Camera(projection), Primary(primary) {}
	};

	struct LightComponent
	{
		Ume::Emission Emission;
		DEFAULT_CONSTRUCTORS(LightComponent);
		LightComponent(const Ume::Emission& emission)
			: Emission(emission) {}
	};

	struct SphereComponent
	{
		glm::vec3 Center = { 0.0f, 0.0f, 0.0f };
		float Radius = 1.0f;
		int MaterialID = 0;
		bool Visible = true;
		DEFAULT_CONSTRUCTORS(SphereComponent);
	};
}