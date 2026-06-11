#pragma once

#include <memory>
#define SDL_MAIN_HANDLED

#include <vulkan/vulkan_raii.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include "Core/Application/Managers/WindowManager.hpp"
#include "Core/Application/Renderer/Renderer.hpp"
#include "System/Base/GameManager.hpp"

namespace Beer::Core
{
    class Application
    {
    private:
        WindowManager windowManager = {};
        Renderer renderer = {};
        std::unique_ptr<System::GameManager> gameManager = nullptr;

    public:
        void Run();

    private:
        void InitializeVulkan();
        void InitializeGame();
        void MainLoop();
        void Cleanup();
    };
} // namespace Beer::Core
