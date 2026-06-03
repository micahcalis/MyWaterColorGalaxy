#include "Core/Assets/MeshAsset.hpp"
#include "RenderPassEvent.hpp"
#include "Rendering/RenderPasses/DrawSkyboxPass.hpp"
#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Shader/Shader.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "Rendering/Shader/ShaderPass.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "Rendering/Texture/TextureMakeSettings.hpp"
#include "System/Camera/Camera.hpp"
#include <cmath>
#include <memory>

namespace Beer::Rendering
{
    static const glm::vec4 SKYBOX_COLOR = glm::vec4(0, 0, 0, 1);
    static const float GRANULATION_NOISE_INTENSITY = 0.5f;
    static const float WETNESS = 1.0f;
    static const float DUST_OPACITY = 0.3f;
    static const float DUST_BLEED_THRESHOLD = 0.1f;

    static const uint32_t NOISE_RESOLUTION = 2048;
    static const VkFormat NOISE_FORMAT = VK_FORMAT_R16G16B16A16_SFLOAT;
    static const uint32_t NOISE_GROUP_SIZE = 8;
    static const uint32_t NOISE_KERNEL = 0;
    static const uint32_t NOISE_DEPTH = 6;
    static const float NOISE_FREQ = 2.75f;
    static const float NOISE_FREQ_MUL = 2.0f;
    static const float NOISE_AMPL_MUL = 0.8f;
    static const float NOISE_BALANCE = 0.55f;
    static const float NOISE_EXP = 2.0f;
    static const float NOISE_EDGE_MIN = 0.2f;
    static const float NOISE_EDGE_MAX = 0.6f;

    static const uint32_t BLUR_KERNEL = 1;
    static const uint32_t NOISE_BLUR_DEPTH = 4;
    static const float NOISE_BLUR_RADIUS = 1.0f;
    static const float NOISE_BLUR_SPREAD_A = 10.0f;
    static const float NOISE_BLUR_SPREAD_B = 100.0f;

    static const uint32_t STAR_COUNT = 10000;
    static const float STAR_SIZE = 5;
    static const glm::vec4 STAR_COLOR = glm::vec4(1);

    DrawSkyboxPass::DrawSkyboxPass(std::shared_ptr<Rendering::Texture3D> controlNoiseVolume)
        : controlNoiseVolume(controlNoiseVolume)
        , IRenderPass("Skybox", RenderPassEvent::SKYBOX)
    {
        auto skyboxShader = Shader::Get("Galaxy/GalaxySkybox");
        skyboxMaterial = std::make_shared<Rendering::Material>(skyboxShader);
        cubeMesh = Mesh::Get("MDL_Cube");
        skyboxTransform.Scale = glm::vec3(800, 800, 800);

        skyboxMaterial->SetColor("_SkyboxColor", SKYBOX_COLOR);
        skyboxMaterial->SetFloat("_GranulationNoiseIntensity", GRANULATION_NOISE_INTENSITY);
        skyboxMaterial->SetFloat("_Wetness", WETNESS);
        skyboxMaterial->SetFloat("_DustOpacity", DUST_OPACITY);
        skyboxMaterial->SetFloat("_BleedThreshold", DUST_BLEED_THRESHOLD);
        skyboxMaterial->SetTexture("_ControlNoiseVolume", controlNoiseVolume.get());

        InitializeStarPointCloud();
    }

    void DrawSkyboxPass::OnRenderSetup(const RenderContext& context)
    {
    }

    void DrawSkyboxPass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        skyboxTransform.Position = System::Camera::Main()->GetTransform()->Position;
        commandBuffer->BindModelPush(skyboxTransform.GetShaderTransform(),
            skyboxMaterial->GetShader());

        const ShaderPass* skyboxPass = skyboxMaterial->GetShader()->GetPass(ShaderPassType::Skybox);
        commandBuffer->BindShaderPass(skyboxMaterial->GetShader(), skyboxPass, context.Output);
        commandBuffer->BindMaterial(skyboxMaterial.get());
        commandBuffer->BindMesh(cubeMesh.get(), &skyboxPass->Input.BufferOrder);

        Rendering::MeshDrawInfo skyboxDrawInfo = cubeMesh->GetDrawInfo();
        commandBuffer->DrawMeshSingle(skyboxDrawInfo);

        commandBuffer->BindModelPush(skyboxTransform.GetShaderTransform(),
            starMaterial->GetShader());

        const ShaderPass* starsPass = starMaterial->GetShader()->GetPass(ShaderPassType::Skybox);
        commandBuffer->BindShaderPass(starMaterial->GetShader(), starsPass, context.Output);
        commandBuffer->BindMaterial(starMaterial.get());
        commandBuffer->BindMesh(starPointCloud.get(), &starsPass->Input.BufferOrder);

        Rendering::MeshDrawInfo starsDrawInfo = starPointCloud->GetDrawInfo();
        commandBuffer->DrawMeshSingle(starsDrawInfo);
    }

    PassDependencyList DrawSkyboxPass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);
        dependencies.AddDependency(PassDependency(std::string(VIRTUAL_MAIN_COLOR),
            ResourceAction::ColorWrite,
            ResetOperator::ClearColor({0, 0, 0, 0}),
            static_cast<vk::Format>(Core::Screen::ColorFormat())));

        dependencies.AddDependency(PassDependency(std::string(GBUFFER_NORMAL_OFFSET),
            ResourceAction::ColorWrite,
            ResetOperator::ClearColor({0, 0, 0.5f, 0.5f}),
            static_cast<vk::Format>(GBUFFER_NORMAL_OFFSET_FORMAT)));

        dependencies.AddDependency(PassDependency(std::string(GBUFFER_WATERCOLOR),
            ResourceAction::ColorWrite,
            ResetOperator::ClearColor({0, 0, 0, 0}),
            static_cast<vk::Format>(GBUFFER_WATERCOLOR_FORMAT)));

        return dependencies;
    }

    void DrawSkyboxPass::InitializeNoiseCubemaps(const System::SerializableGalaxy& serializedGalaxy)
    {
        TextureMakeSettings makeSettings{};
        makeSettings.Width = NOISE_RESOLUTION;
        makeSettings.Height = NOISE_RESOLUTION;
        makeSettings.Depth = 6;
        makeSettings.IsCubeMap = true;
        makeSettings.Format = NOISE_FORMAT;
        makeSettings.KernelIndex = NOISE_KERNEL;
        makeSettings.GroupSizeX = NOISE_GROUP_SIZE;
        makeSettings.GroupSizeY = NOISE_GROUP_SIZE;

        noiseContext = std::make_shared<ComputeContext>("Watercolor/SkyboxNoise");
        noiseContext->SetInt("_Depth", NOISE_DEPTH);
        noiseContext->SetFloat("_Frequency", NOISE_FREQ);
        noiseContext->SetFloat("_FrequencyMultiplier", NOISE_FREQ_MUL);
        noiseContext->SetFloat("_AmplitudeMultiplier", NOISE_AMPL_MUL);
        noiseContext->SetFloat("_NoiseBalance", NOISE_BALANCE);
        noiseContext->SetFloat("_Exponent", NOISE_EXP);
        noiseContext->SetFloat("_EdgeMin", NOISE_EDGE_MIN);
        noiseContext->SetFloat("_EdgeMax", NOISE_EDGE_MAX);
        noiseContext->SetInt("_SeedR", serializedGalaxy.ColorSeed);
        noiseContext->SetInt("_SeedG", serializedGalaxy.ColorSeed + 234);
        noiseContext->SetInt("_SeedB", serializedGalaxy.ColorSeed + 40606);
        skyboxMaterial->SetColor("_ColorA", serializedGalaxy.ColorA);
        skyboxMaterial->SetColor("_ColorB", serializedGalaxy.ColorB);
        skyboxMaterial->SetColor("_ColorC", serializedGalaxy.ColorC);

        noiseCubemap = std::make_shared<Texture2D>(Texture2D::Make(makeSettings,
            noiseContext.get()));

        skyboxMaterial->SetTexture("_DustNoiseCubemap", noiseCubemap.get());

        makeSettings.KernelIndex = BLUR_KERNEL;

        blurNoiseContextA = std::make_shared<ComputeContext>("Watercolor/SkyboxNoise");
        blurNoiseContextA->SetInt("_BlurDepth", NOISE_BLUR_DEPTH);
        blurNoiseContextA->SetFloat("_BlurRadius", 1.0f);
        blurNoiseContextA->SetFloat("_BlurSpread", 2.0f);
        blurNoiseContextA->SetTexture("_BlurSource", noiseCubemap.get());

        blurredNoiseCubemapA = std::make_shared<Texture2D>(Texture2D::Make(makeSettings,
            blurNoiseContextA.get()));

        skyboxMaterial->SetTexture("_BlurredDustNoiseCubemapA", blurredNoiseCubemapA.get());

        blurNoiseContextB = std::make_shared<ComputeContext>("Watercolor/SkyboxNoise");
        blurNoiseContextB->SetInt("_BlurDepth", NOISE_BLUR_DEPTH);
        blurNoiseContextB->SetFloat("_BlurRadius", 5.0f);
        blurNoiseContextB->SetFloat("_BlurSpread", 20.0f);
        blurNoiseContextB->SetTexture("_BlurSource", noiseCubemap.get());

        blurredNoiseCubemapB = std::make_shared<Texture2D>(Texture2D::Make(makeSettings,
            blurNoiseContextB.get()));

        skyboxMaterial->SetTexture("_BlurredDustNoiseCubemapB", blurredNoiseCubemapB.get());

        starMaterial->SetInt("_StarSeed", serializedGalaxy.ColorSeed);
    }

    void DrawSkyboxPass::InitializeStarPointCloud()
    {
        Core::MeshAsset proceduralAsset;

        for (uint32_t i = 0; i < STAR_COUNT; i++)
        {
            float u = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            float v = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

            float theta = 2.0f * 3.14f * u;
            float y = 2.0f * v - 1.0f;

            float r = std::sqrt(1.0f - y * y);

            float x = r * std::cos(theta);
            float z = r * std::sin(theta);

            proceduralAsset.Positions.push_back(glm::vec3(x, y, z));
        }

        starPointCloud = Rendering::Mesh::Create(proceduralAsset);

        starMaterial = std::make_shared<Material>("Galaxy/SkyboxStars");
        starMaterial->SetFloat("_StarSize", STAR_SIZE);
        starMaterial->SetColor("_StarColor", STAR_COLOR);
    }
} // namespace Beer::Rendering
