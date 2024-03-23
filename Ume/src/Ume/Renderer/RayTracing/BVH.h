#pragma once
#include "Ume/Core/Base.h"
#include "BoundingBox.h"
#include "Object.h"
#include "Ray.h"

namespace Ume
{
	struct BVHBuildNode
	{
		BoundingBox Bounds;
		Ref<Ume::Object> Object = nullptr;
		Ref<BVHBuildNode> Left = nullptr;
		Ref<BVHBuildNode> Right = nullptr;
		float Area = 0.0f;
        uint32_t Start = 0;
        uint32_t End = 0;


		BVHBuildNode() = default;
		BVHBuildNode(const BoundingBox& bounds)
			: Bounds(bounds) {}
		BVHBuildNode(const BoundingBox& bounds, const Ref<Ume::Object>& object)
			: Bounds(bounds), Object(object) {}
		
		HitPayload Intersect(const Ray& ray) const;

		inline bool IsLeaf() const { return !Left && !Right; }
	};

	class BVHManager
	{
	public:
		static Ref<BVHBuildNode> RecursiveBuild(std::vector<Ref<Ume::Object>> objects);
    private:
        static Ref<BVHBuildNode> SplitObject(const Ref<Ume::Object>& object, uint32_t start, uint32_t end);
    };
}