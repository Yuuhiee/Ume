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
	struct Triangle
	{
		std::array<Vertex*, 3> Vertices;
		std::array<uint32_t, 3> Indices;
		float Area = 0.0f;

		Triangle() = default;
		Triangle(const std::array<Vertex*, 3>& vertices, const std::array<uint32_t, 3>& indices, float area)
			: Vertices(vertices), Indices(indices), Area(area) {}

		Vertex GetLerpedVertex(float u, float v) const
		{
			return Vertex::Lerp(*Vertices[0], *Vertices[1], *Vertices[2], u, v);
		}

		bool Intersect(const Ray& ray, float& t, float& u, float& v) const
		{
			return Math::RayTriangleIntersect(Vertices[0]->Position, Vertices[1]->Position, Vertices[2]->Position, ray.Origin, ray.Direction, t, u, v);
		}

		BoundingBox GetBounds() const
		{
			BoundingBox bounds(Vertices[0]->Position, Vertices[1]->Position);
			bounds = BoundingBox::Union(bounds, Vertices[2]->Position);
			return bounds;
		}
	};

	class MeshTriangle : public Object
	{
	public:
		std::vector<Vertex> Vertices;
		std::vector<uint32_t> Indices;
		std::vector<Triangle> Triangles;
	public:
		MeshTriangle(const std::string& name, std::vector<Vertex>* vertices, std::vector<uint32_t>* indices);
		~MeshTriangle() = default;
        HitPayload Intersect(const Ray &ray) const override;
        HitPayload Sample(float& pdf) const override;
		inline BoundingBox GetBounds() const override { return m_Bounds; }
		void ResetMesh(const std::string& path) override;

        HitPayload Intersect(const Ray &ray, int start, int end) const;
        BoundingBox GetBounds(int start, int end) const;
        float GetArea(int start, int end) const;
	private:
		std::vector<Vertex>* m_Origin;
		BoundingBox m_Bounds;
		void TransformVertex(Vertex& vertex);
		void InitVertexArray();
		void Rebuild() override;
	};
}