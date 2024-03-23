#pragma once

#include "Ume/Core/Base.h"
#include "Ume/Util/Math.h"
#include "glm/glm.hpp"
#include "Object.h"
#include "BoundingBox.h"
#include "Vertex.h"
#include "Ume/Renderer/VertexArray.h"

namespace Ume
{
	class Sphere : public Object
	{
	public:
		glm::vec3 Center{ 0.0f };
		float Radius = 0.5f;
	public:
		Sphere(const std::string name = "Sphere");
		~Sphere() = default;
		HitPayload Intersect(const Ray& ray) const override;
		HitPayload Sample(float& pdf) const override;
		inline BoundingBox GetBounds() const override { return m_Bounds; }
	private:
		BoundingBox m_Bounds;
		std::vector<Vertex>   Vertices;
		std::vector<uint32_t> Indices;

		void Rebuild() override;
	};
}