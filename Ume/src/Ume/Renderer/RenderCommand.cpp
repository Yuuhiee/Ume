#include "umepch.h"
#include "RenderCommand.h"

namespace Ume
{
	Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();
}