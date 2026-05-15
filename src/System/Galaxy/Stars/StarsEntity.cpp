#include "System/Galaxy/Stars/StarsEntity.hpp"
#include "Core/Assets/MeshAsset.hpp"
#include "Rendering/Mesh/MeshBuffers.hpp"
#include "System/Components/General/SingleMeshRender.hpp"
#include "System/Components/General/Transform.hpp"
#include "System/Components/Registry/GameEntity.hpp"
#include "System/Context/ContextType.hpp"
#include "System/Drawing/RenderRegister.hpp"
#include "vulkan/vulkan.hpp"
#include <memory>

namespace Beer::System
{
    static const float STAR_SIZE = 100.0f;
    static const glm::vec4 STAR_COLOR = glm::vec4(0.8f);

    StarsEntity::StarsEntity(uint32_t starCount,
        float starBoxSize)
        : starCount(starCount)
        , starBoxSize(starBoxSize)
        , GameEntity()
    {
        starsMaterial = std::make_shared<Rendering::Material>("Galaxy/Stars");
        starsMaterial->SetFloat("_StarSize", STAR_SIZE);
        starsMaterial->SetColor("_StarColor", STAR_COLOR);
        starsMaterial->SetFloat("_StarBoxSize", starBoxSize);

        InitializeStarMesh();

        renderComponent = RenderRegister::CreateRenderComponent<SingleMeshRender>(ContextType::Galaxy,
            starsMaterial,
            starsMesh,
            &transform,
            &layer);
    }

    void StarsEntity::InitializeStarMesh()
    {
        Core::MeshAsset proceduralAsset;
        glm::vec3 position;

        for (uint32_t i = 0; i < starCount; i++)
        {
            position.x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * starBoxSize;
            position.y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * starBoxSize;
            position.z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * starBoxSize;

            proceduralAsset.Positions.push_back(position);
        }

        starsMesh = Rendering::Mesh::Create(proceduralAsset);
    }
} // namespace Beer::System
