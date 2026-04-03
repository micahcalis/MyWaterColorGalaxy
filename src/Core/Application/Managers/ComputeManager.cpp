#include "Core/Application/Managers/ComputeManager.hpp"
#include "Core/Application/Utilities/AssetUtilities.hpp"
#include "Rendering/Compute/ComputeShader.hpp"

namespace Beer::Core
{
    std::shared_ptr<Rendering::ComputeShader> ComputeManager::Load(const std::filesystem::path& path)
    {
        std::filesystem::path jsonPath = AssetUtilities::GetShaderJsonPath(path);

        return std::make_shared<Rendering::ComputeShader>(
            path,
            jsonPath,
            *device,
            *swapchain);
    }

    const std::filesystem::path ComputeManager::GetPath(const std::string& name)
    {
        return AssetUtilities::GetComputeShaderPath(name);
    }
} // namespace Beer::Core
