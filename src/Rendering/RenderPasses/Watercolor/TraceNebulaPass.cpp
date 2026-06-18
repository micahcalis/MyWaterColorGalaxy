#include "Rendering/RenderPasses/Watercolor/TraceNebulaPass.hpp"
#include "NebulaBuffer.hpp"
#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependency.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResetOperator.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"
#include "Rendering/Texture/Texture3D.hpp"
#include "Rendering/Texture/TextureMakeSettings.hpp"

namespace Beer::Rendering
{
    static const int32_t MAX_STEPS = 200;
    static const float MAX_LENGTH = 3000.0f;
    static const float STEP_SIZE = 5.0f;
    static const float SHAPE_NOISE_SCALE = 1.0f / 100.0f;
    static const float DETAIL_NOISE_SCALE = 1.0f / 50.0f;
    static const float COVERAGE = 0.8f;
    static const float DETAIL_INTENSITY = 0.5f;
    static const float SHAPE_ABSORPTION = 1.0f;
    static const float LIGHT_ABSORPTION = 1.0f;
    static const float HENYEYK = 0.2f;
    static const float HENYEYG = 0.5f;
    static const float LIGHT_MUL = 6.0f;

    static const uint32_t NOISE_GROUP_SIZE = 8;
    static const uint32_t SHAPE_NOISE_RES = 128;
    static const VkFormat SHAPE_NOISE_FORMAT = VK_FORMAT_R8G8B8A8_UNORM;
    static const uint32_t SHAPE_NOISE_KERNEL = 0;
    static const float SHAPE_NOISE_DEPTH = 7;
    static const float SHAPE_NOISE_SEED = 2823;
    static const float SHAPE_NOISE_FREQ = 2.0f;
    static const float SHAPE_NOISE_FREQ_MUL = 1.5f;
    static const float SHAPE_NOISE_AMPL_MUL = 0.8f;
    static const float SHAPE_NOISE_BALANCE = 0.75f;
    static const float SHAPE_NOISE_EXP = 1.25f;
    static const float SHAPE_NOISE_EDGE_MIN = 0.14f;
    static const float SHAPE_NOISE_EDGE_MAX = 0.7f;

    static const uint32_t DETAIL_NOISE_RES = 64;
    static const VkFormat DETAIL_NOISE_FORMAT = VK_FORMAT_R8G8B8A8_UNORM;
    static const uint32_t DETAIL_NOISE_KERNEL = 0;
    static const float DETAIL_NOISE_DEPTH = 7;
    static const float DETAIL_NOISE_SEED = 709;
    static const float DETAIL_NOISE_FREQ = 1.5f;
    static const float DETAIL_NOISE_FREQ_MUL = 1.5f;
    static const float DETAIL_NOISE_AMPL_MUL = 0.8f;
    static const float DETAIL_NOISE_BALANCE = 0.5f;
    static const float DETAIL_NOISE_EXP = 1.25f;
    static const float DETAIL_NOISE_EDGE_MIN = 0.14f;
    static const float DETAIL_NOISE_EDGE_MAX = 0.7f;

    static const uint32_t DIST_NOISE_RES = 128;
    static const VkFormat DIST_NOISE_FORMAT = VK_FORMAT_R8G8B8A8_UNORM;
    static const uint32_t DIST_NOISE_BASE_KERNEL = 1;
    static const uint32_t DIST_NOISE_CURL_KERNEL = 2;
    static const float DIST_NOISE_DEPTH = 4;
    static const float DIST_NOISE_SEED = 709;
    static const float DIST_NOISE_FREQ = 2.0f;
    static const float DIST_NOISE_FREQ_MUL = 2.0f;
    static const float DIST_NOISE_AMPL_MUL = 0.5f;
    static const float DIST_NOISE_EXP = 1.0f;
    static const float DIST_NOISE_EDGE_MIN = 0.0f;
    static const float DIST_NOISE_EDGE_MAX = 1.0f;

    static const float DIST_NOISE_CURL_EDGE_MIN = 0.415f;
    static const float DIST_NOISE_CURL_EDGE_MAX = 0.63f;

    static const float COL_B_NOISE_DEPTH = 2;
    static const float COL_B_NOISE_SEED = 456;
    static const float COL_B_NOISE_FREQ = 4.0f;
    static const float COL_B_NOISE_FREQ_MUL = 2.0f;
    static const float COL_B_NOISE_AMPL_MUL = 0.5f;
    static const float COL_B_NOISE_EXP = 1.0f;
    static const float COL_B_NOISE_EDGE_MIN = 0.0f;
    static const float COL_B_NOISE_EDGE_MAX = 1.0f;

    static const float COL_S_NOISE_DEPTH = 1;
    static const float COL_S_NOISE_SEED = 456;
    static const float COL_S_NOISE_FREQ = 4.0f;
    static const float COL_S_NOISE_FREQ_MUL = 2.0f;
    static const float COL_S_NOISE_AMPL_MUL = 0.5f;
    static const float COL_S_NOISE_EXP = 1.5f;
    static const float COL_S_NOISE_EDGE_MIN = 0.0f;
    static const float COL_S_NOISE_EDGE_MAX = 1.0f;

    TraceNebulaPass::TraceNebulaPass(NebulaBuffer* nebulaBuffer,
        System::GalaxyObjectBuffer* stardustBuffer)
        : nebulaBuffer(nebulaBuffer)
        , stardustBuffer(stardustBuffer)
        , IRenderPass("TraceNebulaPass", static_cast<uint32_t>(RenderPassEvent::VOLUMETRICS) + 1)
    {
        traceMaterial = std::make_shared<Rendering::Material>("Galaxy/Nebula");
        traceMaterial->SetInt("_MaxRaySteps", MAX_STEPS);
        traceMaterial->SetFloat("_MaxRayLength", MAX_LENGTH);
        traceMaterial->SetFloat("_RayStepSize", STEP_SIZE);
        traceMaterial->SetColor("_TestColor", glm::vec4(0.25, 0.05, 0.4, 0.2f));
        traceMaterial->SetFloat("_ShapeNoiseScale", SHAPE_NOISE_SCALE);
        traceMaterial->SetFloat("_DetailNoiseScale", DETAIL_NOISE_SCALE);
        traceMaterial->SetFloat("_Coverage", COVERAGE);
        traceMaterial->SetFloat("_DetailIntensity", DETAIL_INTENSITY);
        traceMaterial->SetFloat("_ShapeAbsorption", SHAPE_ABSORPTION);
        traceMaterial->SetFloat("_LightAbsorption", LIGHT_ABSORPTION);
        traceMaterial->SetFloat("_HenyeyG", HENYEYG);
        traceMaterial->SetFloat("_HenyeyK", HENYEYK);
        traceMaterial->SetFloat("_LightMultiplier", LIGHT_MUL);

        InitializeNoiseVolumes();

        traceMaterial->SetTexture("_NebulaShapeNoise", shapeNoise.get());
        traceMaterial->SetTexture("_NebulaDetailNoise", detailNoise.get());
        traceMaterial->SetTexture("_NebulaDistortionNoise", distortionNoise.get());
    }

    void TraceNebulaPass::OnRenderSetup(const RenderContext& context)
    {
        nebulaBuffer->ReallocateTiles(context);
        nebulaBuffer->ReallocateTarget(context);

        traceMaterial->SetStructuredBuffer("_DynamicStardustPositions", stardustBuffer->GetPositionBuffer());
        traceMaterial->SetStructuredBuffer("_StardustObjectData", stardustBuffer->GetDataBuffer());
        traceMaterial->SetStructuredBuffer("_NebulaTileBuffer", nebulaBuffer->NebulaTilesBuffer);
        traceMaterial->SetTexture("_DepthBuffer", context.MainDepthTarget);
        traceMaterial->SetInt("_StardustCount", stardustBuffer->GetInstanceCount());

        glm::uvec2 tileCount = NebulaBuffer::GetDispatchTileCount();
        traceMaterial->SetVector("_TileDimensions", glm::vec4(tileCount.x, tileCount.y, 0, 0));
    }

    void TraceNebulaPass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        commandBuffer->Blit(nullptr,
            traceMaterial.get(),
            ShaderPassType::Volumetrics,
            context.Output,
            {stardustBuffer->GetPositionOffset()});
    }

    PassDependencyList TraceNebulaPass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);
        dependencies.AddDependency(PassDependency(NEBULA_TILES_NAME,
            ResourceAction::BufferRead));

        dependencies.AddDependency(PassDependency(NEBULA_TARGET_NAME,
            ResourceAction::ColorWrite,
            ResetOperator::ClearColor({0, 0, 0, 0}),
            static_cast<vk::Format>(NEBULA_TARGET_FORMAT)));

        dependencies.AddDependency(PassDependency(std::string(MAIN_DEPTH),
            ResourceAction::DepthRead,
            ResetOperator::ClearDepth()));

        return dependencies;
    }

    void TraceNebulaPass::InitializeNoiseVolumes()
    {
        TextureMakeSettings makeSettings{};
        makeSettings.Width = SHAPE_NOISE_RES;
        makeSettings.Height = SHAPE_NOISE_RES;
        makeSettings.Depth = SHAPE_NOISE_RES;
        makeSettings.GroupSizeX = NOISE_GROUP_SIZE;
        makeSettings.GroupSizeY = NOISE_GROUP_SIZE;
        makeSettings.GroupSizeZ = NOISE_GROUP_SIZE;
        makeSettings.Format = SHAPE_NOISE_FORMAT;
        makeSettings.KernelIndex = SHAPE_NOISE_KERNEL;

        shapeContext = std::make_shared<ComputeContext>("Watercolor/NebulaNoise");
        shapeContext->SetInt("_DepthR", SHAPE_NOISE_DEPTH);
        shapeContext->SetInt("_SeedR", SHAPE_NOISE_SEED);
        shapeContext->SetFloat("_FrequencyR", SHAPE_NOISE_FREQ);
        shapeContext->SetFloat("_FrequencyMultiplierR", SHAPE_NOISE_FREQ_MUL);
        shapeContext->SetFloat("_AmplitudeMultiplierR", SHAPE_NOISE_AMPL_MUL);
        shapeContext->SetFloat("_NoiseBalanceR", SHAPE_NOISE_BALANCE);
        shapeContext->SetFloat("_ExponentR", SHAPE_NOISE_EXP);
        shapeContext->SetFloat("_EdgeMinR", SHAPE_NOISE_EDGE_MIN);
        shapeContext->SetFloat("_EdgeMaxR", SHAPE_NOISE_EDGE_MAX);

        shapeContext->SetInt("_DepthG", SHAPE_NOISE_DEPTH);
        shapeContext->SetInt("_SeedG", SHAPE_NOISE_SEED + 232);
        shapeContext->SetFloat("_FrequencyG", SHAPE_NOISE_FREQ * 2.0f);
        shapeContext->SetFloat("_FrequencyMultiplierG", SHAPE_NOISE_FREQ_MUL);
        shapeContext->SetFloat("_AmplitudeMultiplierG", SHAPE_NOISE_AMPL_MUL);
        shapeContext->SetFloat("_NoiseBalanceG", SHAPE_NOISE_BALANCE);
        shapeContext->SetFloat("_ExponentG", SHAPE_NOISE_EXP);
        shapeContext->SetFloat("_EdgeMinG", SHAPE_NOISE_EDGE_MIN);
        shapeContext->SetFloat("_EdgeMaxG", SHAPE_NOISE_EDGE_MAX);

        shapeContext->SetInt("_DepthB", SHAPE_NOISE_DEPTH);
        shapeContext->SetInt("_SeedB", SHAPE_NOISE_SEED + 923);
        shapeContext->SetFloat("_FrequencyB", SHAPE_NOISE_FREQ * 8.0f);
        shapeContext->SetFloat("_FrequencyMultiplierB", SHAPE_NOISE_FREQ_MUL);
        shapeContext->SetFloat("_AmplitudeMultiplierB", SHAPE_NOISE_AMPL_MUL);
        shapeContext->SetFloat("_NoiseBalanceB", SHAPE_NOISE_BALANCE);
        shapeContext->SetFloat("_ExponentB", SHAPE_NOISE_EXP);
        shapeContext->SetFloat("_EdgeMinB", SHAPE_NOISE_EDGE_MIN);
        shapeContext->SetFloat("_EdgeMaxB", SHAPE_NOISE_EDGE_MAX);

        shapeContext->SetInt("_DepthA", SHAPE_NOISE_DEPTH);
        shapeContext->SetInt("_SeedA", SHAPE_NOISE_SEED + 923);
        shapeContext->SetFloat("_FrequencyA", SHAPE_NOISE_FREQ * 16.0f);
        shapeContext->SetFloat("_FrequencyMultiplierA", SHAPE_NOISE_FREQ_MUL);
        shapeContext->SetFloat("_AmplitudeMultiplierA", SHAPE_NOISE_AMPL_MUL);
        shapeContext->SetFloat("_NoiseBalanceA", SHAPE_NOISE_BALANCE);
        shapeContext->SetFloat("_ExponentA", SHAPE_NOISE_EXP);
        shapeContext->SetFloat("_EdgeMinA", SHAPE_NOISE_EDGE_MIN);
        shapeContext->SetFloat("_EdgeMaxA", SHAPE_NOISE_EDGE_MAX);

        shapeNoise = std::make_shared<Texture3D>(Texture3D::Make(makeSettings,
            shapeContext.get()));

        makeSettings.Width = DETAIL_NOISE_RES;
        makeSettings.Height = DETAIL_NOISE_RES;
        makeSettings.Depth = DETAIL_NOISE_RES;
        makeSettings.KernelIndex = DETAIL_NOISE_KERNEL;
        makeSettings.Format = DETAIL_NOISE_FORMAT;

        detailContext = std::make_shared<ComputeContext>("Watercolor/NebulaNoise");
        detailContext->SetInt("_DepthR", DETAIL_NOISE_DEPTH);
        detailContext->SetInt("_SeedR", DETAIL_NOISE_SEED);
        detailContext->SetFloat("_FrequencyR", DETAIL_NOISE_FREQ);
        detailContext->SetFloat("_FrequencyMultiplierR", DETAIL_NOISE_FREQ_MUL);
        detailContext->SetFloat("_AmplitudeMultiplierR", DETAIL_NOISE_AMPL_MUL);
        detailContext->SetFloat("_NoiseBalanceR", DETAIL_NOISE_BALANCE);
        detailContext->SetFloat("_ExponentR", DETAIL_NOISE_EXP);
        detailContext->SetFloat("_EdgeMinR", DETAIL_NOISE_EDGE_MIN);
        detailContext->SetFloat("_EdgeMaxR", DETAIL_NOISE_EDGE_MAX);

        detailContext->SetInt("_DepthG", DETAIL_NOISE_DEPTH);
        detailContext->SetInt("_SeedG", DETAIL_NOISE_SEED + 232);
        detailContext->SetFloat("_FrequencyG", DETAIL_NOISE_FREQ * 2.0f);
        detailContext->SetFloat("_FrequencyMultiplierG", DETAIL_NOISE_FREQ_MUL);
        detailContext->SetFloat("_AmplitudeMultiplierG", DETAIL_NOISE_AMPL_MUL);
        detailContext->SetFloat("_NoiseBalanceG", DETAIL_NOISE_BALANCE);
        detailContext->SetFloat("_ExponentG", DETAIL_NOISE_EXP);
        detailContext->SetFloat("_EdgeMinG", DETAIL_NOISE_EDGE_MIN);
        detailContext->SetFloat("_EdgeMaxG", DETAIL_NOISE_EDGE_MAX);

        detailContext->SetInt("_DepthB", DETAIL_NOISE_DEPTH);
        detailContext->SetInt("_SeedB", DETAIL_NOISE_SEED + 923);
        detailContext->SetFloat("_FrequencyB", DETAIL_NOISE_FREQ * 8.0f);
        detailContext->SetFloat("_FrequencyMultiplierB", DETAIL_NOISE_FREQ_MUL);
        detailContext->SetFloat("_AmplitudeMultiplierB", DETAIL_NOISE_AMPL_MUL);
        detailContext->SetFloat("_NoiseBalanceB", DETAIL_NOISE_BALANCE);
        detailContext->SetFloat("_ExponentB", DETAIL_NOISE_EXP);
        detailContext->SetFloat("_EdgeMinB", DETAIL_NOISE_EDGE_MIN);
        detailContext->SetFloat("_EdgeMaxB", DETAIL_NOISE_EDGE_MAX);

        detailContext->SetInt("_DepthA", DETAIL_NOISE_DEPTH);
        detailContext->SetInt("_SeedA", DETAIL_NOISE_SEED + 923);
        detailContext->SetFloat("_FrequencyA", DETAIL_NOISE_FREQ * 16.0f);
        detailContext->SetFloat("_FrequencyMultiplierA", DETAIL_NOISE_FREQ_MUL);
        detailContext->SetFloat("_AmplitudeMultiplierA", DETAIL_NOISE_AMPL_MUL);
        detailContext->SetFloat("_NoiseBalanceA", DETAIL_NOISE_BALANCE);
        detailContext->SetFloat("_ExponentA", DETAIL_NOISE_EXP);
        detailContext->SetFloat("_EdgeMinA", DETAIL_NOISE_EDGE_MIN);
        detailContext->SetFloat("_EdgeMaxA", DETAIL_NOISE_EDGE_MAX);

        detailNoise = std::make_shared<Texture3D>(Texture3D::Make(makeSettings,
            detailContext.get()));

        makeSettings.Width = DIST_NOISE_RES;
        makeSettings.Height = DIST_NOISE_RES;
        makeSettings.Depth = DIST_NOISE_RES;
        makeSettings.KernelIndex = DIST_NOISE_BASE_KERNEL;
        makeSettings.Format = DIST_NOISE_FORMAT;

        distortionContextBase = std::make_shared<ComputeContext>("Watercolor/NebulaNoise");
        distortionContextBase->SetInt("_DepthR", DIST_NOISE_DEPTH);
        distortionContextBase->SetInt("_SeedR", DIST_NOISE_SEED);
        distortionContextBase->SetFloat("_FrequencyR", DIST_NOISE_FREQ);
        distortionContextBase->SetFloat("_FrequencyMultiplierR", DIST_NOISE_FREQ_MUL);
        distortionContextBase->SetFloat("_AmplitudeMultiplierR", DIST_NOISE_AMPL_MUL);
        distortionContextBase->SetFloat("_ExponentR", DIST_NOISE_EXP);
        distortionContextBase->SetFloat("_EdgeMinR", DIST_NOISE_EDGE_MIN);
        distortionContextBase->SetFloat("_EdgeMaxR", DIST_NOISE_EDGE_MAX);

        std::shared_ptr<Texture3D> distortionBase = std::make_shared<Texture3D>(Texture3D::Make(makeSettings,
            distortionContextBase.get()));

        makeSettings.KernelIndex = DIST_NOISE_CURL_KERNEL;

        distortionContextCurl = std::make_shared<ComputeContext>("Watercolor/NebulaNoise");
        distortionContextCurl->SetTexture("_CurlSource", distortionBase.get());
        distortionContextCurl->SetFloat("_EdgeMinR", DIST_NOISE_CURL_EDGE_MIN);
        distortionContextCurl->SetFloat("_EdgeMaxR", DIST_NOISE_CURL_EDGE_MAX);

        distortionContextBase->SetInt("_DepthR", COL_B_NOISE_DEPTH);
        distortionContextBase->SetInt("_SeedR", COL_B_NOISE_SEED);
        distortionContextBase->SetFloat("_FrequencyR", COL_B_NOISE_FREQ);
        distortionContextBase->SetFloat("_FrequencyMultiplierR", COL_B_NOISE_FREQ_MUL);
        distortionContextBase->SetFloat("_AmplitudeMultiplierR", COL_B_NOISE_AMPL_MUL);
        distortionContextBase->SetFloat("_ExponentR", COL_B_NOISE_EXP);
        distortionContextBase->SetFloat("_EdgeMinR", COL_B_NOISE_EDGE_MIN);
        distortionContextBase->SetFloat("_EdgeMaxR", COL_B_NOISE_EDGE_MAX);

        distortionContextBase->SetInt("_DepthG", COL_S_NOISE_DEPTH);
        distortionContextBase->SetInt("_SeedG", COL_S_NOISE_SEED);
        distortionContextBase->SetFloat("_FrequencyG", COL_S_NOISE_FREQ);
        distortionContextBase->SetFloat("_FrequencyMultiplierG", COL_S_NOISE_FREQ_MUL);
        distortionContextBase->SetFloat("_AmplitudeMultiplierG", COL_S_NOISE_AMPL_MUL);
        distortionContextBase->SetFloat("_ExponentG", COL_S_NOISE_EXP);
        distortionContextBase->SetFloat("_EdgeMinG", COL_S_NOISE_EDGE_MIN);
        distortionContextBase->SetFloat("_EdgeMaxG", COL_S_NOISE_EDGE_MAX);

        distortionNoise = std::make_shared<Texture3D>(Texture3D::Make(makeSettings,
            distortionContextCurl.get()));
    }
} // namespace Beer::Rendering
