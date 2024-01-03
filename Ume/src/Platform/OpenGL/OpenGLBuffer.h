#pragma once

#include "Ume/Renderer/Buffer.h"

namespace Ume
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		virtual ~OpenGLVertexBuffer() override;
		virtual void Bind() override;
		virtual void Unbind() override;

		virtual const BufferLayout& GetLayout() const override { return m_Layout; };
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; };
	private:
		uint32_t m_RendererID;
		BufferLayout m_Layout;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		virtual ~OpenGLIndexBuffer() override;
		virtual void Bind() override;
		virtual void Unbind() override;
		inline virtual uint32_t GetCount() override { return m_Count; }
	private:
		uint32_t m_RendererID;
		uint32_t m_Count;
	};
}

