#include "umepch.h"
#include "OpenGLContext.h"

#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "Ume/Core.h"

namespace Ume
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		UME_CORE_ASSERT(windowHandle, "Window handle is null!");
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		UME_CORE_ASSERT(status, "Failed to initialize Glad!");

		UME_CORE_INFO("OpenGL Info | Vendor:\t{}", (const char*)glGetString(GL_VENDOR));
		UME_CORE_INFO("            | Renderer:\t{}", (const char*)glGetString(GL_RENDERER));
		UME_CORE_INFO("            | Version:\t{}", (const char*)glGetString(GL_VERSION));

		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLContext::SwapBuffer()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}