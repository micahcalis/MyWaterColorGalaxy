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
                        attributes.vertices[3 * index.vertex_index + 1],
                        attributes.vertices[3 * index.vertex_index + 2]});

                    if (index.normal_index >= 0)
                    {
                        meshAsset.Normals.push_back({attributes.normals[3 * index.normal_index + 0],
                            attributes.normals[3 * index.normal_index + 1],
                            attributes.normals[3 * index.normal_index + 2]});
                    } else
                    {
                        meshAsset.Normals.push_back({0.0f, 0.0f, 0.0f});
                    }

                    meshAsset.Tangents.push_back({0.0f, 0.0f, 0.0f});

                    if (index.texcoord_index >= 0)
                    {
                        meshAsset.UVs.push_back({attributes.texcoords[2 * index.texcoord_index + 0],
                            1.0f - attributes.texcoords[2 * index.texcoord_index + 1]});
                    } else
                    {
                        meshAsset.UVs.push_back({0.0f, 0.0f});
                    }

                    meshAsset.VertexColors.push_back({1.0f, 1.0f, 1.0f, 1.0f});
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
        meshAsset.UVs.reserve(maxVertices);
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
                    meshAsset.Tangents.push_back({
                        tangent.x,
                        tangent.y,
                        tangent.z,
                    });
                }

                if (mesh->vertex_uv.exists)
                {
                    ufbx_vec2 uv = ufbx_get_vertex_vec2(&mesh->vertex_uv, index);
                    meshAsset.UVs.push_back({uv.x, 1.0f - uv.y});
                } else
                {
                    meshAsset.UVs.push_back({0.0f, 0.0f});
                }

                if (mesh->vertex_color.exists)
                {
                    ufbx_vec4 color = ufbx_get_vertex_vec4(&mesh->vertex_color, index);
                    meshAsset.VertexColors.push_back({color.x, color.y, color.z, 1.0f});
                } else
                {
                    meshAsset.VertexColors.push_back({1.0f, 1.0f, 1.0f, 1.0f});
                }
            }
        }

        meshAsset.Indices.resize(meshAsset.Positions.size());

        ufbx_vertex_stream streams[3] = {};

        streams[0].data = meshAsset.Positions.data();
        streams[0].vertex_size = sizeof(glm::vec3);
        streams[0].vertex_count = meshAsset.Positions.size();

        streams[1].data = meshAsset.UVs.data();
        streams[1].vertex_size = sizeof(glm::vec2);
        streams[1].vertex_count = meshAsset.UVs.size();

        streams[2].data = meshAsset.VertexColors.data();
        streams[2].vertex_size = sizeof(glm::vec4);
        streams[2].vertex_count = meshAsset.VertexColors.size();

        ufbx_error ufbxErr;

        size_t uniqueVertexCount = ufbx_generate_indices(
            streams, 3, meshAsset.Indices.data(), meshAsset.Indices.size(), nullptr, &ufbxErr);

        if (uniqueVertexCount == 0)
        {
            fprintf(stderr, "UFBX Index Generation Failed: %s\n", ufbxErr.description.data);
            exit(1);
        }

        meshAsset.Positions.resize(uniqueVertexCount);
        meshAsset.UVs.resize(uniqueVertexCount);
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
