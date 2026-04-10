#pragma once

#include "FontSettings.hpp"
#include "Rendering/Buffer/Buffer.hpp"
#include "Rendering/Mesh/MeshDrawInfo.hpp"
#include "Rendering/Text/FontAsset.hpp"
#include "System/Components/UI/TextSettings.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "Rendering/Text/TextAlignUtilities.hpp"
#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace Beer::Rendering
{
    class TextBuffer
    {
    private:
        std::shared_ptr<Buffer> posBuffer;
        std::shared_ptr<Buffer> uvBuffer;
        std::shared_ptr<Buffer> indexBuffer;
        uint32_t vertexCount;
        uint32_t indexCount;
        std::string cachedText;

        std::vector<uint32_t> cachedUniCodes;
        std::vector<TextWord> cachedWords;
        std::vector<TextLine> cachedLines;
        std::vector<glm::vec2> cachedPositions;
        std::vector<glm::vec2> cachedUVs;
        std::vector<uint32_t> cachedIndices;

    public:
        TextBuffer();

        Buffer* GetPosBuffer() const { return posBuffer.get(); }
        Buffer* GetUVBuffer() const { return uvBuffer.get(); }
        Buffer* GetIndexBuffer() const { return indexBuffer.get(); }

        void Update(const std::string& text,
            const FontAsset* fontAsset,
            const FontSettings& fontSettings,
            const System::TextSettings& settings,
            const System::UITransform* transform);

        void Reload(const FontAsset* fontAsset,
            const FontSettings& fontSettings,
            const System::TextSettings& settings,
            const System::UITransform* transform)
        {
            Update(cachedText, fontAsset, fontSettings, settings, transform);
        }

        MeshDrawInfo GetDrawInfo() const;

    private:
        void CalculateVertices(const FontAsset* fontAsset,
            const FontSettings& fontSettings,
            const System::TextSettings& settings,
            const System::UITransform* transform);
    };
} // namespace Beer::Rendering
