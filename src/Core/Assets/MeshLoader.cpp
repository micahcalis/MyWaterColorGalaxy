#include "Core/Assets/MeshLoader.hpp"
#include "MeshAsset.hpp"
#include <tiny_obj_loader.h>
#include "Vendor/ufbx/ufbx.h"

// AI Generated
namespace std
{
    struct ObjIndexHash
    {
        size_t operator()(const tinyobj::index_t& idx) const
        {
            // Hash combine the three integer indices
            size_t h1 = std::hash<int>()(idx.vertex_index);
            size_t h2 = std::hash<int>()(idx.normal_index);
            size_t h3 = std::hash<int>()(idx.texcoord_index);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };

    // 2. Define how to compare two tinyobj indices
    struct ObjIndexEqual
    {
        bool operator()(const tinyobj::index_t& a, const tinyobj::index_t& b) const
        {
            return a.vertex_index == b.vertex_index && a.normal_index == b.normal_index && a.texcoord_index == b.texcoord_index;
        }
    };
} // namespace std

namespace Beer::Core
{
    MeshAsset MeshLoader::LoadMesh(const std::filesystem::path& path)
    {
        if (IsObj(path))
        {
            return LoadObj(path);
        } else
        {
            return LoadFBX(path);
        }
    }

    MeshAsset MeshLoader::LoadObj(const std::filesystem::path& path)
    {
        MeshAsset meshAsset;
        tinyobj::attrib_t attributes;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attributes, &shapes, &materials, &warn, &err, path.string().c_str()))
        {
            throw std::runtime_error(warn + err);
        }

        std::unordered_map<tinyobj::index_t, uint32_t, std::ObjIndexHash, std::ObjIndexEqual> uniqueVertices{};

        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                if (!uniqueVertices.contains(index))
                {
                    uint32_t newVertexIndex = static_cast<uint32_t>(meshAsset.Positions.size());
                    uniqueVertices[index] = newVertexIndex;

                    meshAsset.Positions.push_back({attributes.vertices[3 * index.vertex_index + 0],
                        attributes.vertices[3 * index.vertex_index + 2],
                        -attributes.vertices[3 * index.vertex_index + 1]});

                    if (index.normal_index >= 0)
                    {
                        meshAsset.Normals.push_back({attributes.normals[3 * index.normal_index + 0],
                            attributes.normals[3 * index.normal_index + 2],
                            -attributes.normals[3 * index.normal_index + 1]});
                    }

                    if (index.texcoord_index >= 0)
                    {
                        meshAsset.UVs.push_back({attributes.texcoords[2 * index.texcoord_index + 0],
                            1.0f - attributes.texcoords[2 * index.texcoord_index + 1]});
                    }
                }

                meshAsset.Indices.push_back(uniqueVertices[index]);
            }
        }

        return meshAsset;
    }

    MeshAsset MeshLoader::LoadFBX(const std::filesystem::path& path)
    {
        MeshAsset meshAsset;
        ufbx_load_opts opts = {0};

        opts.target_axes = ufbx_axes_right_handed_y_up;
        opts.target_unit_meters = 1.0f;

        ufbx_error error;

        ufbx_scene* scene = ufbx_load_file(path.string().c_str(), &opts, &error);
        if (!scene)
        {
            fprintf(stderr, "Failed to load: %s\n", error.description.data);
            exit(1);
        }

        ufbx_mesh* mesh = scene->meshes.data[0];
        std::vector<uint32_t> triIndices(mesh->max_face_triangles * 3);

        size_t maxVertices = mesh->num_triangles * 3;
        meshAsset.Positions.reserve(maxVertices);
        if (mesh->vertex_normal.exists)
            meshAsset.Normals.reserve(maxVertices);
        if (mesh->vertex_tangent.exists)
            meshAsset.Tangents.reserve(maxVertices);
        if (mesh->vertex_uv.exists)
            meshAsset.UVs.reserve(maxVertices);
        if (mesh->vertex_color.exists)
            meshAsset.VertexColors.reserve(maxVertices);

        for (size_t i = 0; i < mesh->num_faces; i++)
        {
            ufbx_face face = mesh->faces.data[i];
            uint32_t numTris = ufbx_triangulate_face(triIndices.data(), triIndices.size(), mesh, face);

            for (size_t v = 0; v < numTris * 3; v++)
            {
                uint32_t index = triIndices[v];

                ufbx_vec3 pos = ufbx_get_vertex_vec3(&mesh->vertex_position, index);
                meshAsset.Positions.push_back({pos.x, pos.y, pos.z});

                if (mesh->vertex_normal.exists)
                {
                    ufbx_vec3 normal = ufbx_get_vertex_vec3(&mesh->vertex_normal, index);
                    meshAsset.Normals.push_back({normal.x, normal.y, normal.z});
                }

                if (mesh->vertex_tangent.exists)
                {
                    ufbx_vec3 tangent = ufbx_get_vertex_vec3(&mesh->vertex_tangent, index);
                    meshAsset.Tangents.push_back({tangent.x, tangent.y, tangent.z});
                }

                if (mesh->vertex_uv.exists)
                {
                    ufbx_vec2 uv = ufbx_get_vertex_vec2(&mesh->vertex_uv, index);
                    meshAsset.UVs.push_back({uv.x, 1.0f - uv.y});
                }

                if (mesh->vertex_color.exists)
                {
                    ufbx_vec4 color = ufbx_get_vertex_vec4(&mesh->vertex_color, index);
                    meshAsset.VertexColors.push_back({color.x, color.y, color.z, 1.0f});
                }
            }
        }

        meshAsset.Indices.resize(meshAsset.Positions.size());

        std::vector<ufbx_vertex_stream> streams;

        ufbx_vertex_stream posStream = {};
        posStream.data = meshAsset.Positions.data();
        posStream.vertex_size = sizeof(glm::vec3);
        posStream.vertex_count = meshAsset.Positions.size();
        streams.push_back(posStream);

        if (mesh->vertex_normal.exists)
        {
            ufbx_vertex_stream normStream = {};
            normStream.data = meshAsset.Normals.data();
            normStream.vertex_size = sizeof(glm::vec3);
            normStream.vertex_count = meshAsset.Normals.size();
            streams.push_back(normStream);
        }

        if (mesh->vertex_tangent.exists)
        {
            ufbx_vertex_stream tanStream = {};
            tanStream.data = meshAsset.Tangents.data();
            tanStream.vertex_size = sizeof(glm::vec3);
            tanStream.vertex_count = meshAsset.Tangents.size();
            streams.push_back(tanStream);
        }

        if (mesh->vertex_uv.exists)
        {
            ufbx_vertex_stream uvStream = {};
            uvStream.data = meshAsset.UVs.data();
            uvStream.vertex_size = sizeof(glm::vec2);
            uvStream.vertex_count = meshAsset.UVs.size();
            streams.push_back(uvStream);
        }

        if (mesh->vertex_color.exists)
        {
            ufbx_vertex_stream colStream = {};
            colStream.data = meshAsset.VertexColors.data();
            colStream.vertex_size = sizeof(glm::vec4);
            colStream.vertex_count = meshAsset.VertexColors.size();
            streams.push_back(colStream);
        }

        ufbx_error ufbxErr;
        size_t uniqueVertexCount = ufbx_generate_indices(
            streams.data(), streams.size(), meshAsset.Indices.data(), meshAsset.Indices.size(), nullptr, &ufbxErr);

        if (uniqueVertexCount == 0)
        {
            throw std::runtime_error("UFBX Index Generation Failed: " + std::string(ufbxErr.description.data));
        }

        meshAsset.Positions.resize(uniqueVertexCount);
        if (mesh->vertex_normal.exists)
            meshAsset.Normals.resize(uniqueVertexCount);
        if (mesh->vertex_tangent.exists)
            meshAsset.Tangents.resize(uniqueVertexCount);
        if (mesh->vertex_uv.exists)
            meshAsset.UVs.resize(uniqueVertexCount);
        if (mesh->vertex_color.exists)
            meshAsset.VertexColors.resize(uniqueVertexCount);

        ufbx_free_scene(scene);

        return meshAsset;
    }

    bool MeshLoader::IsObj(const std::filesystem::path& path)
    {
        std::string ext = path.extension().string();

        if (ext == ".obj")
        {
            return true;
        } else if (ext == ".fbx")
        {
            return false;
        }

        throw std::runtime_error("Unsupported model format: " + ext + " for path: " + path.string());
    }
} // namespace Beer::Core
