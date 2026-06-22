#pragma once

#include "Rendering/Material/Material.hpp"
#include "System/Components/General/Transform.hpp"
#include "System/Components/Registry/IEntityManager.hpp"

namespace Beer::System
{
    class PlayerVFXManager : public IEntityManager
    {
    private:
        Function<Transform> getPlayerTransformdata = nullptr;
        Function<bool> isBoosting = nullptr;
        Transform* flameTransform = nullptr;
        Rendering::Material* flameMaterial = nullptr;

        bool boostState = false;

    public:
        PlayerVFXManager(Function<Transform> getPlayerTransformdata,
            Function<bool> isBoosting,
            Transform* flameTransform,
            Rendering::Material* flameMaterial)
            : getPlayerTransformdata(getPlayerTransformdata)
            , isBoosting(isBoosting)
            , flameTransform(flameTransform)
            , flameMaterial(flameMaterial)
        {
        }

        void Update() override
        {
            UpdateVFXTransform();
            UpdateVFXMaterial();
        }

    private:
        void UpdateVFXTransform()
        {
            const Transform playerTransform = getPlayerTransformdata();
            flameTransform->Position = playerTransform.Position;
            flameTransform->Rotation = playerTransform.Rotation;
        }

        void UpdateVFXMaterial()
        {
            bool boosting = isBoosting();

            if (boosting != boostState)
            {
                boostState = boosting;
                flameMaterial->SetInt("_BoostState", boostState ? 1 : 0);
            }
        }
    };
} // namespace Beer::System
