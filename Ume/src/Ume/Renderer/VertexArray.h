#pragma once

#include "Buffer.h"
#include "Ume/Renderer/RayTracing/Vertex.h"

namespace Ume
{
	enum class ObjectType
	{
		Cube, Quad, Sphere, Mesh
	};

	static std::vector<const char*> ObjectTypeNames = { "Cube", "Quad", "Sphere", "Mesh"};

	class VertexData
	{
	public:
		static void SetSphere(uint32_t sectors, uint32_t stacks);
		static std::vector<Vertex>*	GetVertexBuffer(ObjectType type);
		static std::vector<uint32_t>* GetIndexBuffer(ObjectType type);

		static std::vector<Vertex>* QuadVB;
		static std::vector<uint32_t>* QuadIB;
		static std::vector<Vertex>* CubeVB;
		static std::vector<uint32_t>* CubeIB;
		static std::vector<Vertex>* SphereVB;
		static std::vector<uint32_t>* SphereIB;
	};

	class VertexArray
	{
	public:
		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;

		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;

		static Ref<VertexArray> Create();
		static Ref<VertexArray> Create(ObjectType type);
	};
}
