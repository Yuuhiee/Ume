#include "umepch.h"
#include "PerspectiveCameraController.h"

#include "Ume/Core/Input.h"
#include "Ume/Core/KeyCodes.h"

namespace Ume
{
    PerspectiveCameraController::PerspectiveCameraController(float fov, float width, float height, float zNear, float zFar)
        : m_FOV(fov), m_ZNear(zNear), m_ZFar(zFar), m_Width(width), m_Height(height)
    {
        if (height == 0)
            m_AspectRatio = 0.0f;
        else
            m_AspectRatio = width / height;
        m_Camera = PerspectiveCamera(fov, width, height, zNear, zFar);
    }

    void PerspectiveCameraController::OnUpdate(Timestep ts)
    {
        if (Input::IsKeyPressed(UME_KEY_W))
            m_Position.z -= m_MoveSpeed * ts;
        if (Input::IsKeyPressed(UME_KEY_S))
            m_Position.z += m_MoveSpeed * ts;
        if (Input::IsKeyPressed(UME_KEY_A))
            m_Position.x -= m_MoveSpeed * ts;
        if (Input::IsKeyPressed(UME_KEY_D))
            m_Position.x += m_MoveSpeed * ts;
        if (Input::IsKeyPressed(UME_KEY_E))
            m_Position.y -= m_MoveSpeed * ts;
        if (Input::IsKeyPressed(UME_KEY_Q))
            m_Position.y += m_MoveSpeed * ts;
        if (Input::IsKeyPressed(UME_KEY_LEFT_SHIFT))
            m_Rotation.y -= m_RotateSpeed * ts;
        if (Input::IsKeyPressed(UME_KEY_F))
            m_Rotation.y += m_RotateSpeed * ts;

        m_Camera.SetPosition(m_Position);
        m_Camera.SetRotation(m_Rotation);
    }

    void PerspectiveCameraController::OnEvent(Event &e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseScrolledEvent>(UME_BIND_EVENT_FN(PerspectiveCameraController::OnMouseScrolled));
        dispatcher.Dispatch<WindowResizeEvent>(UME_BIND_EVENT_FN(PerspectiveCameraController::OnWindowResized));
    }
    bool PerspectiveCameraController::OnMouseScrolled(const MouseScrolledEvent &e)
    {
        m_FOV -= e.GetYOffset();
        m_FOV = std::clamp(m_FOV, 10.0f, 120.0f);
        m_Camera.SetProjection(m_FOV, m_Width, m_Height, m_ZNear, m_ZFar);
        return false;
    }
    bool PerspectiveCameraController::OnWindowResized(const WindowResizeEvent &e)
    {
        m_Width = (float)e.GetWidth();
        m_Height = (float)e.GetHeight();
        m_Camera.SetProjection(m_FOV, m_Width, m_Height, m_ZNear, m_ZFar);
        return false;
    }
}