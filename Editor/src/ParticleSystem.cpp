#include "ParticleSystem.h"

#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

namespace Ume
{
	ParticleSystem::ParticleSystem(uint32_t maxParticles)
		: m_PoolIndex(maxParticles - 1), PoolSize(maxParticles)
	{
		m_ParticlePool.resize(maxParticles);
	}

	void ParticleSystem::Resize(uint32_t maxParticles)
	{
		m_ParticlePool.resize(maxParticles);
		PoolSize = maxParticles;
	}

	void ParticleSystem::OnUpdate(Timestep ts)
	{
		if (m_ParticleProps.Gravity)
		{
			for (auto& particle : m_ParticlePool)
			{
				if (!particle.Active)
					continue;

				if (particle.LifeRemaining <= 0.0f)
				{
					particle.Active = false;
					continue;
				}

				particle.LifeRemaining -= ts;
				particle.Velocity.y -= 10.f * ts;
				particle.Velocity *= 1 - m_ParticleProps.Dampping * ts;
				particle.Position += particle.Velocity * (float)ts;
				particle.Rotation += 0.01f * ts;
			}
		}
		else
		{
			for (auto& particle : m_ParticlePool)
			{
				if (!particle.Active)
					continue;

				if (particle.LifeRemaining <= 0.0f)
				{
					particle.Active = false;
					continue;
				}

				particle.LifeRemaining -= ts;
				particle.Velocity *= 1 - m_ParticleProps.Dampping * ts;
				particle.Position += particle.Velocity * (float)ts;
				particle.Rotation += 0.01f * ts;
			}
		}
	}

	void ParticleSystem::OnRender(const OrthographicCamera& camera)
	{
		Renderer2D::BeginScene(camera);
		for (auto& particle : m_ParticlePool)
		{
			if (!particle.Active)
				continue;

			// Fade away particles
			float life = particle.LifeRemaining / particle.LifeTime;
			glm::vec4 color = glm::lerp(particle.ColorEnd, particle.ColorBegin, life);
			//color.a = color.a * life;

			float size = glm::lerp(particle.SizeEnd, particle.SizeBegin, life);

			Renderer2D::DrawQuad({ particle.Position.x, particle.Position.y, 0.5f }, { size, size }, particle.Rotation, color);
		}
		Renderer2D::EndScene();
	}

	void ParticleSystem::OnRender(const glm::mat4& vp)
	{
		Renderer2D::BeginScene(vp);
		for (auto& particle : m_ParticlePool)
		{
			if (!particle.Active)
				continue;

			// Fade away particles
			float life = particle.LifeRemaining / particle.LifeTime;
			glm::vec4 color = glm::lerp(particle.ColorEnd, particle.ColorBegin, life);
			//color.a = color.a * life;

			float size = glm::lerp(particle.SizeEnd, particle.SizeBegin, life);

			Renderer2D::DrawQuad({ particle.Position.x, particle.Position.y, 0.5f }, { size, size }, particle.Rotation, color);
		}
		Renderer2D::EndScene();
	}

	void ParticleSystem::Emit(const ParticleProps& particleProps)
	{
		m_ParticleProps = particleProps;
		Particle& particle = m_ParticlePool[m_PoolIndex];
		particle.Active = true;
		particle.Position = particleProps.Position;
		particle.Rotation = Random::Float() * 360.0f;
		//particle.Rotation = Random::Float() * 2.0f * glm::pi<float>();

		// Velocity
		particle.Velocity = particleProps.Velocity;
		particle.Velocity.x += particleProps.VelocityVariation.x * (Random::Float() - 0.5f);
		particle.Velocity.y += particleProps.VelocityVariation.y * (Random::Float() - 0.5f);

		// Color
		particle.ColorBegin = particleProps.ColorBegin;
		particle.ColorEnd = particleProps.ColorEnd;

		particle.LifeTime = particleProps.LifeTime;
		particle.LifeRemaining = particleProps.LifeTime;
		particle.SizeBegin = particleProps.SizeBegin + particleProps.SizeVariation * (Random::Float() - 0.5f);
		particle.SizeEnd = particleProps.SizeEnd;

		m_PoolIndex = --m_PoolIndex % m_ParticlePool.size();
	}
}