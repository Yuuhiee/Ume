#include "umepch.h"
#include "RTCamera.h"

#include "Ume/Core/Input.h"
#include "Ume/Core/KeyCodes.h"
#include "glm/gtx/euler_angles.hpp"

namespace Ume
{
	Camera::Camera(float verticalFOV, uint32_t width, uint32_t height, float nearClip, float farClip)
		: FoV(verticalFOV), m_NearClip(nearClip), m_FarClip(farClip)
	{
		Forawrd = glm::vec3(0, 0, -1);
		Position = glm::vec3(0, 0, 6);
		Resize(width, height);
	}

	void Camera::Recalculate()
	{
		Pitch = std::clamp(Pitch, -89.0f, 89.0f);
		glm::mat4 rotationMatrix = glm::yawPitchRoll(glm::radians(Yaw), glm::radians(Pitch), glm::radians(0.0f));
		Forawrd = glm::normalize(glm::vec3(rotationMatrix[2]));
		Right = -glm::normalize(glm::vec3(rotationMatrix[0]));
		Up = glm::normalize(glm::vec3(rotationMatrix[1]));
		RecalculateProjection();
		RecalculateView();
		m_ViewProjection = m_Projection * m_View;
	}

	bool Camera::Resize(uint32_t width, uint32_t height)
	{
		if (width == m_ViewportWidth && height == m_ViewportHeight)
			return false;

		m_ViewportWidth = width;
		m_ViewportHeight = height;
		m_PixelSize = { 1.0f / (float)m_ViewportWidth, 1.0f / (float)m_ViewportHeight };

		RecalculateProjection();
		m_ViewProjection = m_Projection * m_View;
		return true;
	}

	void Camera::RecalculateProjection()
	{
		m_Projection = glm::perspectiveFov(glm::radians(FoV), (float)m_ViewportWidth, (float)m_ViewportHeight, m_NearClip, m_FarClip);
		m_InverseProjection = glm::inverse(m_Projection);
	}

	void Camera::RecalculateView()
	{
		m_View = glm::lookAt(Position, Position + Forawrd, glm::vec3(0, 1, 0));
		m_InverseView = glm::inverse(m_View);
	}

	void RTCamera::RecalculateRayDirections()
	{
		m_RayDirections.resize(m_ViewportWidth * m_ViewportHeight);

		float scale = tan(glm::radians(FoV * 0.5f));
		float imageAspectRatio = (float)m_ViewportWidth / (float)m_ViewportHeight;
		float k = scale * imageAspectRatio;

		for (uint32_t y = 0; y < m_ViewportHeight; y++)
		{
			for (uint32_t x = 0; x < m_ViewportWidth; x++)
			{
				glm::vec2 coord = glm::vec2((float)x / (float)m_ViewportWidth, (float)y / (float)m_ViewportHeight) * 2.0f - 1.0f;
				glm::vec4 target = m_InverseProjection * glm::vec4(coord, -1, 1);
				glm::vec3 dir = m_InverseView * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0); 
				m_RayDirections[x + y * m_ViewportWidth] = dir;
			}
		}
	}

	void EditorCameraController::Update(Timestep ts)
	{
		auto [x, y] = Input::GetMousePosition();
		glm::vec2 mousePos = { x, y };
		static glm::vec2 lastMousePosition;
		glm::vec2 delta = (mousePos - lastMousePosition) * 0.002f;
		lastMousePosition = mousePos;

		if (!Input::IsMouseButtonPressed(UME_MOUSE_BUTTON_2))
		{
			Input::SetCursorMode(CursorMode::Normal);
			return;
		}

		Input::SetCursorMode(CursorMode::Locked);

		bool moved = false;

		// Movement
		glm::vec3 direction = glm::vec3(0.0f);
		if (Input::IsKeyPressed(UME_KEY_W)) direction += m_Camera->Forawrd;
		if (Input::IsKeyPressed(UME_KEY_S)) direction -= m_Camera->Forawrd;
		if (Input::IsKeyPressed(UME_KEY_A)) direction -= m_Camera->Right;
		if (Input::IsKeyPressed(UME_KEY_D)) direction += m_Camera->Right;
		if (Input::IsKeyPressed(UME_KEY_Q)) direction -= glm::vec3(0.0f, 1.0f, 0.0f);
		if (Input::IsKeyPressed(UME_KEY_E)) direction += glm::vec3(0.0f, 1.0f, 0.0f);
		if (direction != glm::vec3(0.0f))
		{
			m_Camera->Position += direction * m_Camera->MoveSpeed * (float)ts;
			moved = true;
		}

		// Rotation
		if (delta.x != 0.0f || delta.y != 0.0f)
		{
			m_Camera->Yaw -= delta.x * m_Camera->Sensitivity * 100.0f;
			m_Camera->Pitch += delta.y * m_Camera->Sensitivity * 100.0f;
			moved = true;
		}

		if (moved)
		{
			m_Camera->Recalculate();
		}

	}
}