#include "umepch.h"
#include "BVH.h"
#include "MeshTriangle.h"
#include "Ume/Util/Random.h"

namespace Ume
{
	static uint32_t MaxFacesLeaf = 64;
	HitPayload BVHBuildNode::Intersect(const Ray& ray) const
	{
		if (!this) return {};
		if (!Bounds.Intersect(ray)) return {};
		if (IsLeaf())
		{
			if (!Object->Active()) return {};
			if (End == 0)
				return Object->Intersect(ray);
			return std::dynamic_pointer_cast<MeshTriangle>(Object)->Intersect(ray, Start, End);
		}
		return std::min(Left->Intersect(ray), Right->Intersect(ray));
	}

	Ref<BVHBuildNode>  BVHManager::RecursiveBuild(std::vector<Ref<Ume::Object>> objects)
	{
		int n = (int)objects.size();
		BoundingBox bounds;
		for (const auto& object : objects)
			bounds = BoundingBox::Union(bounds, object->GetBounds());

		Ref<BVHBuildNode>  root = CreateRef<BVHBuildNode>(bounds);

		if (n == 1)
		{
			auto& object = objects[0];
            if (object->Type == ObjectType::Mesh)
            {
                auto mesh = std::dynamic_pointer_cast<MeshTriangle>(object);
                uint32_t faces = mesh->Triangles.size();
				if (faces > MaxFacesLeaf)
                {
                    root->Left = SplitObject(object, 0, faces / 2);
                    root->Right = SplitObject(object, faces / 2, faces);
                }
            }
            root->Object = object;
            root->Area = object->Area;
        }
		else if (n == 2)
		{
			root->Left = RecursiveBuild({ objects[0] });
			root->Right = RecursiveBuild({ objects[1] });
			root->Area = objects[0]->Area + objects[1]->Area;
		}
		else
		{
			BoundingBox centroidBounds;
			for (const auto& object : objects)
				centroidBounds = BoundingBox::Union(centroidBounds, object->GetBounds().Centroid());
			int dim = centroidBounds.LongestAxis();
			std::sort(objects.begin(), objects.begin(), [dim](auto&& f1, auto&& f2)
				{
					return f1->GetBounds().Centroid()[dim] < f2->GetBounds().Centroid()[dim];
				});

			auto begin = objects.begin();
			auto middle = begin + objects.size() / 2;
			auto end = objects.end();
			root->Left = RecursiveBuild({ begin, middle});
			root->Right = RecursiveBuild({ middle, end });
			root->Area = root->Left->Area + root->Right->Area;
		}

		return root;
	}

    Ref<BVHBuildNode> BVHManager::SplitObject(const Ref<Ume::Object> &object, uint32_t start, uint32_t end)
    {
        auto mesh = std::dynamic_pointer_cast<MeshTriangle>(object);
        Ref<BVHBuildNode> node = CreateRef<BVHBuildNode>(mesh->GetBounds(start, end));
		node->Area = mesh->GetArea(start, end);
        if (end - start <= MaxFacesLeaf)
        {
            node->Object = object;
            node->Start = start;
            node->End = end;
            return node;
        }
        auto middle = start + (end - start) / 2;
        node->Left = SplitObject(object, start, middle);
        node->Right = SplitObject(object, middle, end);
		return node;
    }
}
