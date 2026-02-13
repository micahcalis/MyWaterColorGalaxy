#include "Core/Application/Application.hpp"
#include "Core/Application/Utilities/SDLUtilities.hpp"
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
        InitializeVulkan();
        MainLoop();
        Cleanup();
    }

    void Application::InitializeVulkan()
    {
        renderer.InitializeVulkanInstances(windowManager.GetWindow());
    }

    void Application::MainLoop()
    {
        bool isRunning = true;
        bool isResized = false;
        SDL_Event event;

        // 1. The "Game Loop" - Runs every frame
        while (isRunning)
        {
            // 2. The "Event Loop" - Processes all input for this frame
            //    (We loop until PollEvent returns 0, meaning "queue empty")
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
            }

            // 3. Handle Resize (Bridge from SDL to Renderer)
            if (isResized)
            {
                renderer.SetFrameBufferResized(true);
                isResized = false;
            }

            // 4. Draw - Happens even if the user isn't touching the keyboard/mouse
            if (isRunning)
            {
                renderer.Draw();
            }
        }

        renderer.GetDevice().GetLogicalDevice().waitIdle();
    }
    void Application::Cleanup()
    {
        std::cout << "cleanup" << std::endl;
        windowManager.Cleanup();
        renderer.GetSwapchain().CleanupSwapchain();
    }
} // namespace Beer::Core
