#pragma once
#include "Ume/Core/Base.h"
#include "Ume/Core/Timestep.h"
#include "Ume/Renderer/Texture.h"
#include "Ume/Renderer/Framebuffer.h"
#include "Ume/Scene/Scene.h"
#include "SampleMethod.h"

namespace Ume
{
	enum class SampleType
	{
		ImportanceCos, ImportanceGGX, LightImportance, MultiImportance
	};
	static std::vector<const char*> SampleTypeNames = { "ImportanceCos", "ImportanceGGX", "LightImportance", "MultiImportance" };

	enum class LightPickType
	{
		None, All, Random,
	};
	static std::vector<const char*> LightPickTypeNames = { "None", "All", "Random" };

	class RTRenderer
	{
	public:
		struct Configuration
		{
			int SPP = 1;
			float RR = 0.8f;
			SampleType Method = SampleType::ImportanceGGX;
			bool Env = false;
			LightPickType PickLight = LightPickType::Random;
			float Rate1 = 0.8f;
			float Rate2 = 0.1f;
			bool Denoise = false;
			bool Normal = false;
			bool Color = false;
			bool Plane = false;
			float wp = 0.0f;
			float wn = 0.0f;
			float wc = 0.0f;
			float wpl = 0.0f;
			bool Accumulate = true;
			bool Antilias = true;
			SampleMethodFlags Methods = SampleMethodFlagBits::ImportanceGGX | SampleMethodFlagBits::ImportanceLight;
		};
		static Configuration Config;
		static Ref<Texture2D>	s_PositionImage;
		static Ref<Texture2D>	s_NormalImage;
		static Ref<Texture2D>	s_IDImage;
		static uint32_t Frame;
	public:
		static const Ref<Texture2D>& GetImage();
		static void Init(uint32_t width, uint32_t height);
		static void Resize(uint32_t width, uint32_t height);
		static void Render(Timestep ts);
		static void Clear();
		static void GBufferPass(const glm::mat4& vp);
		static inline void Submit(Ref<Scene> scene) { s_Scene = scene; }
		static const Ref<Framebuffer>& GetGBuffer() { return s_GFrameuffer; }

		using Color = glm::vec4;
		using PixelFormat = Color;
	private:
		static bool s_Denoise;
		static Ref<Scene> s_Scene;
		static Ref<Framebuffer> s_PreviousFramebuffer;
		static Ref<Framebuffer> s_CurrentFramebuffer;
		static Ref<Framebuffer> s_FinalFramebuffer;
		static Ref<Framebuffer> s_GFrameuffer;
		static std::vector<PixelFormat> s_Data;
		static std::vector<uint32_t> s_HorizontalIter, s_VerticalIter;

		static glm::vec4 RayGen(glm::vec2 coord, uint32_t x, uint32_t y);
		static glm::vec3 Shade(HitPayload& point, const glm::vec3& inDir);
		static glm::vec3 Shade(HitPayload& point, const glm::vec3& inDir, SampleMethodFlags method);
		static glm::vec3 GetIrradiance(const Material& material, const glm::vec3& lightPos, const glm::vec3& receivePos, bool rangeCheck = true);
		
		static void TraceScene();
		static void Denoise();
	};
}