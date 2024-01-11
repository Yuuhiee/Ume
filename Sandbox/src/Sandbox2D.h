#pragma once

#include <Ume.h>

#include "glm/glm.hpp"

class Sandbox2D : public Ume::Layer
{
public:
	Sandbox2D();
	~Sandbox2D() = default;

	virtual void OnUpdate(Ume::Timestep ts) override;
	virtual void OnEvent(Ume::Event& e) override;
	virtual void OnImGuiRender() override;
private:
	Ume::OrthographicCameraController m_CameraController;
	Ume::Ref<Ume::Texture2D> m_Texture;
};
