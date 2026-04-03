#pragma once

#include <type_traits>

namespace Beer::Rendering
{
    class IRenderResource;
    class RenderTexture;
    class PhaseBuffer;

    template<typename T>
    struct ReallocData
    {
        static_assert(std::is_base_of_v<IRenderResource, T>, "Requested type must inherit from IRenderResource!");

    public:
        T* AllocPointer;
        bool IsReallocated;

    public:
        ReallocData(T* allocPointer, bool isReallocated)
            : AllocPointer(allocPointer), IsReallocated(isReallocated)
        {
        }
    };

    using ReallocRT = ReallocData<RenderTexture>;
    using ReallocPB = ReallocData<PhaseBuffer>;
} // namespace Beer::Rendering
