#pragma once

#include "System/Components/Registry/GameEntity.hpp"

namespace Beer::System
{
    class StarsEntity : public GameEntity
    {
    private:
        std::shared_ptr<Rendering::Mesh> starsMesh = nullptr;
        std::shared_ptr<Rendering::Material> starsMaterial = nullptr;

        uint32_t starCount = 0;
        float starBoxSize = 0;

    public:
        StarsEntity(uint32_t starCount,
            float starBoxSize);

        void Update() override
        {
        }

    private:
        void InitializeManager() override
        {
        }

        void InitializeStarMesh();
    };
} // namespace Beer::System
