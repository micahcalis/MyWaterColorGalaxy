#pragma once

#include <vector>

namespace Beer::System
{
    class ILight;
}

namespace Beer::System
{
    class LightManager
    {
    private:
        std::vector<ILight*> lights;

    public:
        void Update();
        void AddLight(ILight* light);
        void RemoveLight(ILight* light);
        ILight* GetMainLight() const;

    private:
        void SortLights();
    };
} // namespace Beer::System
