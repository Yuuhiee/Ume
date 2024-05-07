#include "umepch.h"
#include "VertexArray.h"

#include "Ume/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

static std::vector<Ume::Vertex> quadVertices = {
	{ -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f },
	{  1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f },
	{  1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f },
	{ -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f }
};
static std::vector<uint32_t> quadIndices = {
	0, 1, 2, 2, 3, 0
};

static std::vector<Ume::Vertex> cubeVertices = {
		 {-0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  0.0f, 0.0f},
		 { 0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  1.0f, 0.0f},
		 { 0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  1.0f, 1.0f},
		 {-0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  0.0f, 1.0f},

		 {-0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  1.0f, 0.0f},
		 { 0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 0.0f},
		 { 0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 1.0f},
		 {-0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  1.0f, 1.0f},

		 {-0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 0.0f},
		 {-0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f},
		 {-0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 1.0f},
		 {-0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f},

		 { 0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,  1.0f, 0.0f},
		 { 0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 0.0f},
		 { 0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f},
		 { 0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,  1.0f, 1.0f},

		 {-0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  0.0f, 0.0f},
		 { 0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  1.0f, 0.0f},
		 { 0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  1.0f, 1.0f},
		 {-0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  0.0f, 1.0f},

		 {-0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  0.0f, 0.0f},
		 { 0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  1.0f, 0.0f},
		 { 0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,  1.0f, 1.0f},
		 {-0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,  0.0f, 1.0f}
};
static std::vector<uint32_t> cubeIndices = {
	0,  1,  2 , 2,  3,  0 ,
	4,  5,  6 , 6,  7,  4 ,
	8,  9,  10, 10, 11, 8 ,
	12, 13, 14, 14, 15, 12,
	16, 17, 18, 18, 19, 16,
	20, 21, 22, 22, 23, 20
};

static uint32_t s_Sectors = 36;
static uint32_t s_Stacks = 36;

namespace Ume
{
	std::vector<Vertex>*	VertexData::QuadVB{ &quadVertices };
	std::vector<uint32_t>*	VertexData::QuadIB{ &quadIndices };
	std::vector<Vertex>*	VertexData::CubeVB{ &cubeVertices };
	std::vector<uint32_t>*	VertexData::CubeIB{ &cubeIndices };
	std::vector<Vertex>*	VertexData::SphereVB;
	std::vector<uint32_t>*	VertexData::SphereIB;

	static void BuildSphere(std::vector<Vertex>*& vertices, std::vector<uint32_t>*& indices, uint32_t sectors = 36, uint32_t stacks = 36)
	{
		if (indices && sectors * stacks * 6 == indices->size()) return;
		UME_INFO("REBUILD");

		delete[] vertices;
		delete[] indices;

		vertices = new std::vector<Vertex>;
		indices = new std::vector<uint32_t>;
		vertices->reserve((stacks + 1) * (sectors + 1));
		indices->reserve(stacks * sectors * 6);

		for (uint32_t i = 0; i <= stacks; ++i) {
			float stack_angle = PI / 2.0f - i * PI / stacks; // 纬度角度
			float xy = cos(stack_angle); // 纬度对应的xy值

			for (uint32_t j = 0; j <= sectors; ++j) {
				float sector_angle = j * 2.0f * PI / sectors; // 经度角度

				// 球体顶点坐标
				float x = xy * cos(sector_angle);
				float y = xy * sin(sector_angle);
				float z = sin(stack_angle);

				glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));

				Ume::Vertex vertex;
				vertex.Position = glm::vec3(x, y, z) * 0.5f;
				vertex.Normal = normal;
				vertex.Texcoord = {
					0.5f + atan2f(normal.x, normal.z) * INV_TWO_PI,
					0.5f - asinf(normal.y) * INV_PI
				};
				vertices->push_back(vertex);
			}
		}

		// 计算球体索引
		for (uint32_t i = 0; i < stacks; ++i) {
			for (uint32_t j = 0; j < sectors; ++j) {
				auto first = i * (sectors + 1) + j;
				auto second = first + sectors + 1;

				// 添加三角形索引
				indices->push_back(first);
				indices->push_back(second);
				indices->push_back(first + 1);

				indices->push_back(second);
				indices->push_back(second + 1);
				indices->push_back(first + 1);
			}
		}
	}

	void VertexData::SetSphere(uint32_t sectors, uint32_t stacks)
	{
		s_Sectors = sectors;
		s_Stacks = stacks;
	}
	std::vector<Vertex>* VertexData::GetVertexBuffer(ObjectType type)
	{
		switch (type)
		{
		case ObjectType::Cube: return CubeVB;
		case ObjectType::Quad: return QuadVB;
		case ObjectType::Sphere: BuildSphere(SphereVB, SphereIB); return SphereVB;
		}
		return CubeVB;
	}
	std::vector<uint32_t>* VertexData::GetIndexBuffer(ObjectType type)
	{
		switch (type)
		{
		case ObjectType::Cube: return CubeIB;
		case ObjectType::Quad: return QuadIB;
		case ObjectType::Sphere: BuildSphere(SphereVB, SphereIB); return SphereIB;
		}
		return CubeIB;
	}
}


namespace Ume
{
	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
				UME_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
				return nullptr;
			case RendererAPI::API::OpenGL:
				return CreateRef<OpenGLVertexArray>();
		}

		UME_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}

	Ref<VertexArray> VertexArray::Create(ObjectType type)
	{
		BufferLayout layout = {
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float3, "a_Normal"},
			{ShaderDataType::Float2, "a_TexCoord"},
		};
		auto& vbData = *VertexData::GetVertexBuffer(type);
		auto& ibData = *VertexData::GetIndexBuffer(type);
		auto vb = VertexBuffer::Create(vbData.data(), sizeof(Vertex) * vbData.size());
		auto ib = IndexBuffer::Create(ibData.data(), ibData.size());
		vb->SetLayout(layout);

		Ref<VertexArray> va = VertexArray::Create();
		va->AddVertexBuffer(vb);
		va->SetIndexBuffer(ib);
		return va;
	}
}