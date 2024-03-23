#pragma once

#include "glm/glm.hpp"
#include "Ray.h"
#include "BoundingBox.h"
#include "Vertex.h"
#include "Material.h"
#include "Ume/Renderer/VertexArray.h"
#include "Ume/Scene/Component.h"

namespace Ume
{
	class Object;

	struct HitPayload
	{
		bool Hit = false;
		Ume::Vertex Vertex;
		float Distance = std::numeric_limits<float>::max();
		int ObjectID;
		Ume::Object* Object;

		bool operator<(const HitPayload& rhs) const
		{
			return Distance < rhs.Distance;
		}
	};

	class Object
	{
	public:
		uint32_t ID = 0;
		Ume::Material* Material = nullptr;
		bool Visible = true;
		Ume::Transform Transform;
		std::string Name = "Object";
		ObjectType Type = ObjectType::Mesh;
		float Area = 0.0f;
		std::string MeshPath = "";
	public:
		Object() = default;
		Object(const std::string name) 
			: Name(name) {}
		virtual ~Object() = default;
        virtual HitPayload Intersect(const Ray &ray) const = 0;
        virtual BoundingBox GetBounds() const = 0;
		inline void SetMaterial(Ume::Material* material) { Material = material; }
		inline const Ref<Ume::VertexArray>& GetVertexArray() const { return m_VertexArray; }
		inline float GetArea() const { return Area; }
		virtual HitPayload Sample(float& pdf) const = 0;
		bool Emissive() const { return Material && Material->Emissive && Material->Emission->Intensity > 0.0f ; }
		bool Active() const { return Visible; }
		virtual void ResetMesh(const std::string& path) { MeshPath = path; }
		void SetTransform(const Ume::Transform& transform);
		bool Build();
        void ScaleToFit(uint32_t screenWidth, uint32_t screenHeight);
    protected:
        Ref<Ume::VertexArray> m_VertexArray;
		bool NeedRebuild = false;
		virtual void Rebuild() = 0;
	public:
		static Ref<Object> Create(const std::string& name, ObjectType type);
		static Ref<Object> Create(const std::string& name, const std::string& meshPath);
	};
}