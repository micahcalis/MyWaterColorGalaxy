#pragma once

#include <functional>
#include <vector>

namespace Beer::Core
{
    class RenderGarbageCollector
    {
        friend class Renderer;

    private:
        inline static std::vector<std::vector<std::move_only_function<void()>>> renderGarbage;
        inline static uint32_t frameIndex = 0;

    private:
        static void Initialize(uint32_t framesInFlight)
        {
            renderGarbage.resize(framesInFlight);
        }

        static void SetFrameIndex(uint32_t frameIndex)
        {
            RenderGarbageCollector::frameIndex = frameIndex;
        }

        static void FlushCurrentFrame()
        {
            if (!renderGarbage.empty() && frameIndex < renderGarbage.size())
            {
                for (auto& deletor : renderGarbage[frameIndex])
                {
                    deletor();
                }
                renderGarbage[frameIndex].clear();
            }
        }

        static void DestroyAll()
        {
            for (auto& queue : renderGarbage)
            {
                for (auto& deletor : queue)
                {
                    deletor();
                }
                queue.clear();
            }
            renderGarbage.clear();
        }

    public:
        static void Push(std::move_only_function<void()>&& deletor)
        {
            if (!renderGarbage.empty() && frameIndex < renderGarbage.size())
            {
                renderGarbage[frameIndex].push_back(std::move(deletor));
            } else
            {
                deletor();
            }
        }
    };
} // namespace Beer::Core
