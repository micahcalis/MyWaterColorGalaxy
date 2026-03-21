#pragma once

#include "vulkan/vulkan.hpp"
#include "System/Camera/CameraType.hpp"

namespace Beer::System
{
    struct CameraSettings
    {
    public:
        CameraType type;
        float fieldOfView;
        float nearMin;
        float farMax;
        uint32_t priority;
    };

    static const CameraSettings CAMERA_DEFAULT_SETTINGS = {
        CameraType::Perspective,
        60.0f,
        0.1f,
        1500.0f,
        0};
} // namespace Beer::System
