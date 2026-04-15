#pragma once

#include "Rendering/Uniforms/UniformDescriptor.hpp"
#include <cstdint>
#include <unordered_set>

namespace Beer::Rendering
{
    template<typename T>
    class IDirtyTracker
    {
    protected:
        uint32_t dirtyFramesCountBuffer = 0;
        static inline std::unordered_set<IDirtyTracker*> dirtyQueue;

    public:
        virtual ~IDirtyTracker()
        {
            dirtyQueue.erase(this);
        }

        virtual void Update() = 0;

        static void UpdateDirty()
        {
            for (auto it = dirtyQueue.begin(); it != dirtyQueue.end();)
            {
                T* dirtyObj = static_cast<T*>(*it);

                dirtyObj->Update();

                if (!dirtyObj->IsDirty())
                {
                    it = dirtyQueue.erase(it);
                } else
                {
                    ++it;
                }
            }
        }

        virtual bool IsDirty() const = 0;

    protected:
        virtual void MarkDirty()
        {
            dirtyFramesCountBuffer = UniformDescriptor::GetFramesInFlight();
            dirtyQueue.insert(this);
        }
    };
} // namespace Beer::Rendering
