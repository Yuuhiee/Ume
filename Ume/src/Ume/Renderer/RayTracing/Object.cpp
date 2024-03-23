#include "umepch.h"
#include "Object.h"
#include "AssetManager.h"
#include "MeshTriangle.h"
#include "Sphere.h"
#include "AssetManager.h"
#include "Ume/Util/File.h"
#include "Material.h"

namespace Ume
{
    void Object::SetTransform(const Ume::Transform& transform)
    {
        if (Transform.ModelMatrix != transform.ModelMatrix)
        {
            Transform = transform;
            NeedRebuild = true;
        }
    }

    bool Object::Build()
    {
        //if (NeedRebuild)
        //{
        //    Rebuild();
        //    NeedRebuild = false;
        //    return true;
        //}
        //return false;
        Rebuild();
        return true;
    }

    void Object::ScaleToFit(uint32_t screenWidth, uint32_t screenHeight)
    {
        float diagonal = glm::length(GetBounds().Diagonal());
        float screenDiagonal = sqrt(screenWidth * screenWidth + screenHeight * screenHeight);
        float scale = screenDiagonal / diagonal;
        UME_INFO(diagonal);
        UME_INFO(screenDiagonal);
        UME_INFO(scale);
        Transform.SetScale(glm::vec3(scale));
    }

    Ref<Object> Object::Create(const std::string& name, ObjectType type)
    {
        auto vb = VertexData::GetVertexBuffer(type);
        auto ib = VertexData::GetIndexBuffer(type);
        Ref<Object> object = nullptr;
        switch (type)
        {
        case ObjectType::Cube: 
        case ObjectType::Quad:     object = CreateRef<MeshTriangle>(name, vb, ib); break;
        case ObjectType::Sphere:   object = CreateRef<Sphere>(name); break;
        case ObjectType::Mesh:    object = CreateRef<MeshTriangle>(name, new std::vector<Vertex>, new std::vector<uint32_t>); break;
        }
        if (object)
        {
            object->Type = type;
            object->SetMaterial(new Ume::Material());
        }
        return object;
    }

    Ref<Object> Object::Create(const std::string& name, const std::string& meshPath)
    {
        auto [vertices, indices] = File::LoadMesh(meshPath);
        auto object = CreateRef<MeshTriangle>(name, vertices, indices);
        object->Name = name;
        object->Type = ObjectType::Mesh;
        object->MeshPath = meshPath;
        return object;
    }
}
