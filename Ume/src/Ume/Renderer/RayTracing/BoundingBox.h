#pragma once
#include "Ume/Core/Base.h"
#include "glm/glm.hpp"
#include "Ray.h"

namespace Ume
{
	static glm::vec3 MinVec3(const glm::vec3& p1, const glm::vec3& p2)
	{
		return { std::min(p1.x, p2.x), std::min(p1.y, p2.y), std::min(p1.z, p2.z) };
	}

	static glm::vec3 MaxVec3(const glm::vec3& p1, const glm::vec3& p2)
	{
		return { std::max(p1.x, p2.x), std::max(p1.y, p2.y), std::max(p1.z, p2.z) };
	}

	class BoundingBox
	{
	public:
		glm::vec3 Min;
		glm::vec3 Max;
	public:
		BoundingBox()
		{
			constexpr float minNum = std::numeric_limits<float>::lowest();
			constexpr float maxNum = std::numeric_limits<float>::max();
			Max = glm::vec3(minNum);
			Min = glm::vec3(maxNum);
		}
		BoundingBox(const glm::vec3& point) : Min(point), Max(point) {}
		BoundingBox(const glm::vec3& p1, const glm::vec3& p2)
		{
			Min = MinVec3(p1, p2);
			Max = MaxVec3(p1, p2);
		}

		inline glm::vec3 Diagonal() const { return Max - Min; }

		int LongestAxis() const
		{
			auto d = Diagonal();
			if (d.x > d.y && d.x > d.z) return 0;
			else if (d.y > d.z) return 1;
			return 2;
		}

		bool Intersect(const Ray& ray) const
		{
			glm::vec3 tMin = (Min - ray.Origin) * ray.DirectionInv;
			glm::vec3 tMax = (Max - ray.Origin) * ray.DirectionInv;

			if (ray.Direction.x < 0) std::swap(tMin.x, tMax.x);
			if (ray.Direction.y < 0) std::swap(tMin.y, tMax.y);
			if (ray.Direction.z < 0) std::swap(tMin.z, tMax.z);

			float t_enter = std::max({ tMin.x, tMin.y, tMin.z });
			float t_exit = std::min({ tMax.x, tMax.y, tMax.z });

			return t_enter <= t_exit && t_exit > 0;
		}

		float SurfaceArea() const
		{
			auto d = Diagonal();
			return 2.0f * (d.x * d.y + d.x * d.z + d.y * d.z);
		}

		glm::vec3 Centroid() const { return 0.5f * (Min + Max); }
	public:
		static BoundingBox Intersect(const BoundingBox& b1, const BoundingBox& b2)
		{
			return { MaxVec3(b1.Min, b2.Min), MinVec3(b1.Max, b2.Max) };
		}

		static BoundingBox Union(const BoundingBox& b1, const BoundingBox& b2)
		{
			return { MinVec3(b1.Min, b2.Min), MaxVec3(b1.Max, b2.Max) };
		}

		static BoundingBox Union(const BoundingBox& b, const glm::vec3& p)
		{
			return { MinVec3(b.Min, p), MaxVec3(b.Max, p) };
		}

		static bool Overlaped(const BoundingBox& b1, const BoundingBox& b2)
		{
			bool x = (b1.Max.x >= b2.Min.x) && (b1.Min.x <= b2.Max.x);
			bool y = (b1.Max.y >= b2.Min.y) && (b1.Min.y <= b2.Max.y);
			bool z = (b1.Max.z >= b2.Min.z) && (b1.Min.z <= b2.Max.z);
			return x && y && z;
		}
	};
}