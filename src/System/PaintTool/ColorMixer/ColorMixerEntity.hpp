#pragma once

#include "System/Components/UI/UITransform.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
namespace Beer::System
{
    class ColorMixerEntity : public QuadTreeEntity
    {
    private:
        std::shared_ptr<Rendering::Material> colorMixerDisplayMat;

    public:
        ColorMixerEntity();
        Rendering::Material* GetColorMixerMat() const { return colorMixerDisplayMat.get(); }

    protected:
        void InitializeManager() override
        {
        }

        std::vector<UIRenderItem> GetRenderItems() override
        {
            std::vector<UIRenderItem> renderItems;
            renderItems.push_back(UIRenderItem(&rootTransform, colorMixerDisplayMat.get()));

            return renderItems;
        }
    };
} // namespace Beer::System
