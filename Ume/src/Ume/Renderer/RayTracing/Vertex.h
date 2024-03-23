#pragma once

#include "glm/glm.hpp"
#include "Ume/Util/Math.h"

namespace Ume
{
	struct Vertex
	{
		glm::vec3 Position{ 0.0f };
		glm::vec3 Normal{ 0.0f };
		glm::vec2 Texcoord{ 0.0f };
		//glm::vec3 Tangent{ 0.0f };
		//glm::vec3 Binormal{ 0.0f };

		Vertex() = default;
		Vertex(float x, float y, float z, float nx, float ny, float nz, float u, float v)
			: Position(x, y, z), Normal(nx, ny, nz), Texcoord(u, v) {}

		static Vertex Lerp(const Vertex& v0, const Vertex& v1, const Vertex& v2, float u, float v)
		{
			Vertex res;

			res.Position = Math::TriangleLerp(v0.Position, v1.Position, v2.Position, u, v);
			res.Normal	 = Math::TriangleLerp(v0.Normal,   v1.Normal,   v2.Normal,   u, v);
			res.Texcoord = Math::TriangleLerp(v0.Texcoord, v1.Texcoord, v2.Texcoord, u, v);

			return res;
		}

		void Log() const
		{
			auto& p = Position;
			auto& n = Normal;
			auto& t = Texcoord;
			UME_INFO("P({0}, {1}, {2}), N({3}, {4}, {5}), UV({6}, {7})", p.x, p.y, p.z, n.x, n.y, n.z, t.x, t.y);
		}
	};
}