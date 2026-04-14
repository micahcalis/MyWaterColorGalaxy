#include "System/Components/Colliders/QuadColliderManager.hpp"
#include "QuadCollider.hpp"

namespace Beer::System
{
    QuadColliderManager::QuadColliderManager(Function<MouseInput> getMouseInput)
        : getMouseInput(getMouseInput)
    {
        registry = std::make_unique<QuadColliderRegistry>();
        QuadColliderRegistry::SetQuadColliderRegistry(registry.get());
    }

    void QuadColliderManager::Update()
    {
        registry->Cleanup();

        const MouseInput mouseInput = getMouseInput();
        QuadCollider* selectedCollider = GetSelectedCollider(mouseInput.PixelPos);

        if (selectedCollider != previousCollider)
        {
            if (previousCollider != nullptr)
            {
                previousCollider->OnColliderExit(mouseInput);
            }

            if (selectedCollider != nullptr)
            {
                selectedCollider->OnColliderEntry(mouseInput);
            }
        }

        if (selectedCollider != nullptr)
        {
            selectedCollider->OnColliderActive(mouseInput);
        }

        previousCollider = selectedCollider;
    }

    QuadCollider* QuadColliderManager::GetSelectedCollider(const glm::vec2 mousePixelPos)
    {
        std::vector<QuadCollider*>& colliders = registry->GetColliders();
        QuadCollider* bestHit = nullptr;

        for (auto& collider : colliders)
        {
            if (collider->Hit(mousePixelPos))
            {
                if (bestHit == nullptr || collider->GetDepth() < bestHit->GetDepth())
                {
                    bestHit = collider;
                }
            }
        }

        return bestHit;
    }
} // namespace Beer::System
