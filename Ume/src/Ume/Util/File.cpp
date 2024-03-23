#include "umepch.h"
#include "File.h"
#include "stb_image.h"
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>
#include "stb_image_write.h"

namespace Ume
{
	unsigned char* File::UmeLoadImage(const std::string& path, int& width, int& height, int& channels, bool flip)
	{
		stbi_set_flip_vertically_on_load(flip);
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		UME_CORE_ASSERT(data, "Failed to load image!");
		return data;
	}

    namespace {
        const unsigned int ImportFlags =
            aiProcess_CalcTangentSpace |
            aiProcess_Triangulate |
            aiProcess_SortByPType |
            aiProcess_PreTransformVertices |
            aiProcess_GenNormals |
            aiProcess_GenUVCoords |
            aiProcess_OptimizeMeshes |
            aiProcess_Debone |
            aiProcess_ValidateDataStructure;
    }

    struct LogStream : public Assimp::LogStream
    {
        static void Initialize()
        {
            if (Assimp::DefaultLogger::isNullLogger())
            {
                Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
                Assimp::DefaultLogger::get()->attachStream(new LogStream, Assimp::Logger::Err | Assimp::Logger::Warn);
            }
        }

        void write(const char* message) override
        {
            UME_CORE_ERROR("Assimp error: {0}", message);
        }
    };

    std::pair<std::vector<Vertex>*, std::vector<uint32_t>*> File::LoadMesh(const std::string& path)
	{
        LogStream::Initialize();

        UME_CORE_INFO("Loading mesh: {0}", path);

        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(path, ImportFlags);
        if (!scene || !scene->HasMeshes())
            UME_CORE_ERROR("Failed to load mesh file: {0}", path);

        aiMesh* mesh = scene->mMeshes[0];
        UME_CORE_ASSERT(mesh->HasNormals(), "Meshes require normals.");
        UME_CORE_ASSERT(mesh->HasPositions(), "Meshes require positions.");

        auto vertices = new std::vector<Vertex>();
        vertices->reserve(mesh->mNumVertices);

        for (size_t i = 0; i < vertices->capacity(); i++)
        {
            Vertex vertex;
            vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
            vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

            //if (mesh->HasTangentsAndBitangents())
            //{
            //    vertex.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
            //    vertex.Binormal = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
            //}

            if (mesh->HasTextureCoords(0))
                vertex.Texcoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
            vertices->push_back(vertex);
        }

        auto indices = new std::vector<uint32_t>();
        indices->reserve(mesh->mNumFaces * 3);
        UME_INFO(mesh->mNumFaces * 3);

        for (uint32_t i = 0; i < mesh->mNumFaces; i++)
        {
            UME_CORE_ASSERT(mesh->mFaces[i].mNumIndices == 3, "Must have 3 indices.");
            indices->push_back(mesh->mFaces[i].mIndices[0]);
            indices->push_back(mesh->mFaces[i].mIndices[1]);
            indices->push_back(mesh->mFaces[i].mIndices[2]);
        }

        return { vertices, indices };
	}
    void File::SaveTexture(const Ref<Texture2D>& texture, const std::string& name)
    {
        auto&& pixels = texture->GetTextureData();
        stbi_flip_vertically_on_write(true);
        stbi_write_png(name.c_str(), texture->GetWidth(), texture->GetHeight(), 4, pixels.data(), texture->GetWidth() * 4);
    }
}