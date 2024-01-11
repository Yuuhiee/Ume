#include "umepch.h"
#include "Renderer2D.h"

#include "RenderCommand.h"
#include "Renderer.h"

#include "VertexArray.h"
#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Ume
{
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
		glm::vec4 Color;
		float TexIndex;
	};

	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 10;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32;
		const glm::vec4 QuadVertexPositions[4] = {
			{ -0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f,  0.5f, 0.0f, 1.0f },
			{ -0.5f,  0.5f, 0.0f, 1.0f },
		};
		const glm::vec2 QuadVertexTexCoords[4] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f },
		};

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> TextureShader;
		Ref<Texture> WhiteTexture;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		std::array<Ref<Texture>, MaxTextureSlots> TextureSlots;
		uint32_t TextureInsertIndex = 1;

		Renderer2D::Stastistics Stats;
	};

	static Renderer2DData s_Data;

	void Renderer2D::Init()
	{
		UME_PROFILE_FUNCTION();

		//Renderer::GetShaderLibrary()->Load("assets/shaders/FlatColor.glsl");
		Renderer::GetShaderLibrary()->Load("assets/shaders/BatchQuad.glsl");
		s_Data.TextureShader = Renderer::GetShaderLibrary()->Get("BatchQuad");

		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(whiteTextureData));

		s_Data.QuadVertexArray = VertexArray::Create();

		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
		s_Data.QuadVertexBuffer->SetLayout({
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float2, "a_TexCoord"},
			{ShaderDataType::Float4, "a_Color"},
			{ShaderDataType::Float, "a_TexIndex"},
		});
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];
		for (uint32_t i = 0, offset = 0; i < s_Data.MaxIndices; i += 6, offset += 4)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;
		}
		Ref<IndexBuffer> indexBuffer(IndexBuffer::Create(quadIndices, s_Data.MaxIndices));
		s_Data.QuadVertexArray->SetIndexBuffer(indexBuffer);
		delete[] quadIndices;

		s_Data.TextureSlots[0] = s_Data.WhiteTexture;

		int* textureSlots = new int[s_Data.MaxTextureSlots];
		for (int i = 0; i < s_Data.MaxTextureSlots; i++)
			textureSlots[i] = i;
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetIntArray("u_Textures", textureSlots, s_Data.MaxTextureSlots);
	}

	void Renderer2D::StartScene(const Camera& camera)
	{
		UME_PROFILE_FUNCTION();

		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
		s_Data.TextureInsertIndex = 1;
	}

	void Renderer2D::EndScene()
	{
		UME_PROFILE_FUNCTION();

		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

		Flush();

		s_Data.Stats.DrawCalls++;
	}

	void Renderer2D::Flush()
	{
		UME_PROFILE_FUNCTION();
		for (uint32_t i = 0; i < s_Data.TextureInsertIndex; i++)
			s_Data.TextureSlots[i]->Bind(i);

		RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
	}

	void Renderer2D::FlushAndReset()
	{
		EndScene();

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
		s_Data.TextureInsertIndex = 1;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		UME_PROFILE_FUNCTION();

		DrawQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture)
	{
		UME_PROFILE_FUNCTION();

		DrawQuad({ position.x, position.y, 0.0f }, size, rotation, texture);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		UME_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount >= s_Data.MaxIndices)
			FlushAndReset();

		float textureIndex = 0.0f;

		glm::mat4 model = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		for (int i = 0; i < 4; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = model * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->TexCoord = s_Data.QuadVertexTexCoords[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;

		/*glm::mat4 model = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("u_Model", model);
		s_Data.TextureShader->SetFloat4("u_Color", color);

		s_Data.WhiteTexture->Bind(0);
		s_Data.QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray);*/
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture)
	{
		UME_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount >= s_Data.MaxIndices)
			FlushAndReset();

		float textureIndex = 0.0f;

		for (uint32_t i = 1; i < s_Data.TextureInsertIndex; i++)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0)
		{
			textureIndex = (float)s_Data.TextureInsertIndex;
			s_Data.TextureSlots[s_Data.TextureInsertIndex] = texture;
			s_Data.TextureInsertIndex++;
		}

		glm::vec4 color(1.0f);

		glm::mat4 model = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		for (int i = 0; i < 4; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = model * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->TexCoord = s_Data.QuadVertexTexCoords[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;

		/*glm::mat4 model = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("u_Model", model);
		s_Data.TextureShader->SetFloat4("u_Color", glm::vec4(1.0f));

		texture->Bind(0);
		s_Data.QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray);*/
	}

	Renderer2D::Stastistics Renderer2D::GetStastistics() { return s_Data.Stats; }
	void Renderer2D::ResetStats() { memset(&s_Data.Stats, 0, sizeof(Stastistics)); }
}