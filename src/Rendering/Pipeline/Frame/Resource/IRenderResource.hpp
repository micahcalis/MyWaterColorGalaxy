#pragma once

#include "RenderResourceType.hpp"
namespace Beer::Rendering
{
    class IRenderResource
    {
    protected:
        RenderResourceType type;

    public:
        virtual ~IRenderResource() = default;
        virtual RenderResourceType GetType() const { return type; }
    };
} // namespace Beer::Rendering
