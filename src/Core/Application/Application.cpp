#include "Core/Application/Application.hpp"
#include "System/Base/Input/CursorMode.hpp"
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_events.h>
#include <cassert>
#include <iostream>

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

namespace Beer::Core
{
    constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    void Application::Run()
    {
        windowManager.InitializeWindow();
        System::Cursor::SetWindowManager(&windowManager);
        InitializeVulkan();
        InitializeGame();
        MainLoop();
        Cleanup();
    }

    void Application::InitializeVulkan()
    {
        renderer.InitializeVulkanInstances(windowManager.GetWindow());
    }

    void Application::InitializeGame()
    {
        gameManager.Initialize();
    }

    void Application::MainLoop()
    {
        bool isRunning = true;
        bool isResized = false;
        SDL_Event event;

        while (isRunning)
        {
            if (isRunning)
            {
                gameManager.PreUpdate();
            }

            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_EVENT_QUIT)
                {
                    isRunning = false;
                }

                if (event.type == SDL_EVENT_WINDOW_RESIZED)
                {
                    isResized = true;
                }

                if (event.type == SDL_EVENT_MOUSE_WHEEL)
                {
                    gameManager.OnMouseScrolled(event.wheel.y);
                }
            }

            if (isResized)
            {
                renderer.SetFrameBufferResized(true);
                isResized = false;
            }

            if (isRunning)
            {
                gameManager.Update();
                renderer.PreDraw();
                renderer.Draw();
            }
        }

        renderer.GetDevice().GetLogicalDevice().waitIdle();
    }

    void Application::Cleanup()
    {
        std::cout << "cleanup" << '\n';
        windowManager.Cleanup();
        renderer.GetSwapchain()->CleanupSwapchain();
    }
} // namespace Beer::Core
