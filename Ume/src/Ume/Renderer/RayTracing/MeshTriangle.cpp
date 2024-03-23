#include "umepch.h"
#include "MeshTriangle.h"
#include "Ume/Util/File.h"

namespace Ume
{
	MeshTriangle::MeshTriangle(const std::string& name, std::vector<Vertex>* vertices, std::vector<uint32_t>* indices)
		: Object(name), Vertices(*vertices), Indices(*indices), m_Origin(vertices)
	{
		InitVertexArray();
	}

	void MeshTriangle::TransformVertex(Vertex& vertex)
	{
		vertex.Position = Transform.ModelMatrix * glm::vec4(vertex.Position, 1.0f);
		vertex.Normal = Transform.NormalMatrix * glm::vec4(vertex.Normal, 1.0f);
		vertex.Normal = glm::normalize(vertex.Normal);
	}

	void MeshTriangle::InitVertexArray()
	{
		BufferLayout layout = {
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float3, "a_Normal"},
			{ShaderDataType::Float2, "a_TexCoord"},
		};
		auto vb = VertexBuffer::Create(Vertices.data(), sizeof(Vertex) * Vertices.size());
		auto ib = IndexBuffer::Create(Indices.data(), Indices.size());
		vb->SetLayout(layout);
		m_VertexArray = VertexArray::Create();
		m_VertexArray->AddVertexBuffer(vb);
		m_VertexArray->SetIndexBuffer(ib);
	}

	HitPayload MeshTriangle::Intersect(const Ray& ray) const
	{
		return Intersect(ray, 0, Triangles.size());
	}

	HitPayload MeshTriangle::Intersect(const Ray& ray, int start, int end) const
	{
		float t = std::numeric_limits<float>::max();
		float u = 0.0f, v = 0.0f;
		bool hit = false;

		const Triangle* tri;
		for (int i = start; i < end; ++i)
		{
			float tt, ut, vt;
			if (Triangles[i].Intersect(ray, tt, ut, vt) && tt < t)
			{
				hit = true;
				t = tt;
				u = ut;
				v = vt;
				tri = &Triangles[i];
			}
		}

		HitPayload payload;
		if (hit)
		{
			payload.Hit = true;
			payload.Distance = t;
			payload.Vertex = tri->GetLerpedVertex(u, v);
			payload.Vertex.Position += payload.Vertex.Normal * 0.0001f;
			payload.Object = (Object*)this;
		}
		return payload;
	}

	HitPayload MeshTriangle::Sample(float& pdf) const
	{
		float p = Random::Float() * Area;
		float sum = 0.0f;
		for (const auto& triangle : Triangles)
		{
			sum += triangle.Area;
			if (sum >= p)
			{
				pdf = 1.0f / Area;
				float u = Random::Float();
				float v = Random::Float() * (1.0f - u);
				HitPayload res;
				res.Vertex = triangle.GetLerpedVertex(u, v);
				return res;
			}
		}
		return {};
	}

	void MeshTriangle::Rebuild()
	{
		auto& origin = *m_Origin;
		if (Vertices.size() != origin.size()) Vertices.resize(origin.size());
		for (size_t i = 0; i < Vertices.size(); i++)
		{
			auto vertex = origin[i];
			TransformVertex(vertex);
			Vertices[i] = vertex;
			m_Bounds = BoundingBox::Union(m_Bounds, vertex.Position);
		}

		if (Triangles.size() != Indices.size() / 3)
		{
			Triangles.reserve(Indices.size() / 3);
			for (uint32_t i = 0; i < (uint32_t)Indices.size(); i += 3)
			{
				auto i0 = i;
				auto i1 = i + 1;
				auto i2 = i + 2;
				auto v0 = &Vertices[Indices[i0]];
				auto v1 = &Vertices[Indices[i1]];
				auto v2 = &Vertices[Indices[i2]];
				float area = glm::length(glm::cross(v1->Position - v0->Position, v2->Position - v0->Position)) * 0.5f;
				Triangles.emplace_back(std::array<Vertex*, 3>{ v0, v1, v2 }, std::array{ i0, i1, i2 }, area);
				Area += area;
			}
		}
	}

	void MeshTriangle::ResetMesh(const std::string& path)
	{
		Object::ResetMesh(path);
		auto [vertices, indices] = File::LoadMesh(path);
		Vertices = *vertices;
		Indices = *indices;
		m_Origin = vertices;
		InitVertexArray();
		UME_INFO(Indices.size());
	}

    BoundingBox MeshTriangle::GetBounds(int start, int end) const
    {
        BoundingBox bounds;
        for (int i = start; i < end; i++)
            bounds = BoundingBox::Union(bounds, Triangles[i].GetBounds());
        return bounds;
    }

    float MeshTriangle::GetArea(int start, int end) const
    {
        float area = 0.0f;
        for (int i = start; i < end; ++i)
            area += Triangles[i].Area;
        return area;
    }
}
