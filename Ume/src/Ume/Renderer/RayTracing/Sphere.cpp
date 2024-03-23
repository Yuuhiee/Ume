#include "umepch.h"
#include "Sphere.h"
#include "Ume/Util/Random.h"

namespace Ume
{
	static glm::vec3 OriginCenter = glm::vec3(0.0f);
	static float OriginRadius = 0.5f;

	Sphere::Sphere(const std::string name)
  		: Object(name)
	{
		m_VertexArray = Ume::VertexArray::Create(ObjectType::Sphere);
	}

	void Sphere::Rebuild()
	{
		Center = Transform.ModelMatrix * glm::vec4(OriginCenter, 1.0f);
		Radius = (Transform.Scale * glm::vec3(OriginRadius)).x;

		glm::vec3 halfDiag = glm::vec3(Radius);
		m_Bounds = { Center - halfDiag, Center + halfDiag };
		Area = FOUR_PI * Radius * Radius;
	}

	HitPayload Sphere::Intersect(const Ray& ray) const
	{
		glm::vec3 ori = ray.Origin - Center;
		auto& dir = ray.Direction;
		float a = glm::dot(dir, dir);
		float b = 2.0f * glm::dot(dir, ori);
		float c = glm::dot(ori, ori) - Radius * Radius;

		float discriminant = b * b - 4.0f * a * c;
		if (discriminant < 0) return {};

		float t1 = (-b + sqrt(discriminant)) / (2.0f * a);
		float t2 = (-b - sqrt(discriminant)) / (2.0f * a);

		if (t1 < 0) return {};

		HitPayload payload;
		payload.Hit = true;
		payload.Object = (Object*)this;
		payload.Distance = t2 < 0 ? t1 : t2;
		payload.Vertex.Position = ray.Origin + payload.Distance * ray.Direction;
		payload.Vertex.Normal = glm::normalize(payload.Vertex.Position - Center);
		payload.Vertex.Position += 0.0001f * payload.Vertex.Normal;
		payload.Vertex.Texcoord = { 
			0.5f + atan2f(payload.Vertex.Normal.x, payload.Vertex.Normal.z) * INV_TWO_PI,
			0.5f - asinf(payload.Vertex.Normal.y) * INV_PI
		};

		return payload;
	}

	HitPayload Sphere::Sample(float& pdf) const
	{
		auto dir = Random::Sphere();
		HitPayload payload;
		payload.Vertex.Position = Center + Radius * dir;
		payload.Vertex.Normal = dir;
		pdf = 1.0f / Area;
		return payload;
	}
}