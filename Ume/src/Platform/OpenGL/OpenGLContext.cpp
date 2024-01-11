#include "umepch.h"
#include "OpenGLContext.h"

#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "Ume/Core/Base.h"

namespace Ume
{
    OpenGLContext::OpenGLContext(GLFWwindow *windowHandle)
        : m_WindowHandle(windowHandle)
    {
        UME_CORE_ASSERT(windowHandle, "Window handle is null!");
    }

    void OpenGLContext::Init()
    {
        glfwMakeContextCurrent(m_WindowHandle);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        UME_CORE_ASSERT(status, "Failed to initialize Glad!");

        UME_CORE_INFO("OpenGL Info:");
        UME_CORE_INFO("    Vendor  |{}", (const char *)glGetString(GL_VENDOR));
        UME_CORE_INFO("    Renderer|{}", (const char *)glGetString(GL_RENDERER));
        UME_CORE_INFO("    Version |{}", (const char *)glGetString(GL_VERSION));
    }

    void OpenGLContext::SwapBuffer()
    {
        glfwSwapBuffers(m_WindowHandle);
    }

    void OpenGLContext::Resize(float width, float height)
    {
        glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    }
}