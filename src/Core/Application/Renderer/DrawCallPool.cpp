#include "Core/Application/Renderer/DrawCallPool.hpp"
#include "System/Drawing/BindHistory.hpp"
#include "System/Drawing/BindMask.hpp"
#include "System/Drawing/BindType.hpp"

namespace Beer::Core
{
    void DrawCallPool::BindDrawCalls()
    {
        System::BindHistory history = System::BindHistory(nullptr, nullptr, nullptr);
        System::BindMask mask = System::BindMask(System::BindType::All);

        for (auto component : renderComponents)
        {
            mask = component->GetBindMask(history);
            history = component->Bind(mask, commandBuffer, renderContext, shaderPass);
        }
    }
} // namespace Beer::Core
