#include "Core/Application/Managers/ShaderManager.hpp"
#include "Core/Application/Utilities/AssetUtilities.hpp"
#include "Rendering/Shader/Shader.hpp"

namespace Beer::Core
{
    std::shared_ptr<Rendering::Shader> ShaderManager::Load(const std::filesystem::path& path)
    {
        std::filesystem::path jsonPath = AssetUtilities::GetShaderJsonPath(path);

        return std::make_shared<Rendering::Shader>(
            path,
            jsonPath,
            *device,
            *swapchain);
    }

    void ShaderManager::Initialize()
    {
    }

    void ShaderManager::UpdateGlobals()
    {
        // globalsHandler.Upload(0);
        // globalsHandler.Upload(0);
    }

    const std::filesystem::path ShaderManager::GetPath(const std::string& name)
    {
        return AssetUtilities::GetShaderPath(name);
    }
} // namespace Beer::Core

