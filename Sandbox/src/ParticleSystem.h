#pragma once

#include <Ume.h>

using namespace Ume;

struct ParticleProps
{
	glm::vec2 Position;
	glm::vec2 Velocity, VelocityVariation;
	glm::vec4 ColorBegin, ColorEnd;
	float SizeBegin, SizeEnd, SizeVariation;
	float LifeTime = 1.0f;
	bool Gravity = false;
	float Dampping = 0.01f;
};

class ParticleSystem
{
public:
	ParticleSystem(uint32_t maxParticles = 1000);

	void OnUpdate(Timestep ts);
	void OnRender(const Camera& camera);

	void Emit(const ParticleProps& particleProps);
private:
	struct Particle
	{
		glm::vec2 Position;
		glm::vec2 Velocity;
		glm::vec4 ColorBegin, ColorEnd;
		float Rotation = 0.0f;
		float SizeBegin, SizeEnd;

		float LifeTime = 1.0f;
		float LifeRemaining = 0.0f;

		bool Active = false;
	};
	std::vector<Particle> m_ParticlePool;
	uint32_t m_PoolIndex = 999;
	ParticleProps m_ParticleProps;
};