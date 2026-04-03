#pragma once

#include <cstdint>

namespace Beer::Rendering
{
    struct Threads
    {
    public:
        uint32_t X = 1;
        uint32_t Y = 1;
        uint32_t Z = 1;

    public:
        static Threads GetProportional3D(const uint32_t resolution, const uint32_t groupSize)
        {
            uint32_t threadGroups = GetThreadGroups(resolution, groupSize);

            Threads threads{};
            threads.X = threadGroups;
            threads.Y = threadGroups;
            threads.Z = threadGroups;

            return threads;
        }

        static Threads GetProportional2D(const uint32_t resolution, const uint32_t groupSize)
        {
            uint32_t threadGroups = GetThreadGroups(resolution, groupSize);

            Threads threads{};
            threads.X = threadGroups;
            threads.Y = threadGroups;

            return threads;
        }

        static Threads Get1D(const uint32_t resolution, const uint32_t groupSize)
        {
            uint32_t threadGroups = GetThreadGroups(resolution, groupSize);

            Threads threads{};
            threads.X = threadGroups;

            return threads;
        }

        static Threads GetCustom(const uint32_t resolutionX = 1,
            const uint32_t groupSizeX = 1,
            const uint32_t resolutionY = 1,
            const uint32_t groupSizeY = 1,
            const uint32_t resolutionZ = 1,
            const uint32_t groupSizeZ = 1)
        {
            Threads threads{};
            threads.X = GetThreadGroups(resolutionX, groupSizeX);
            threads.Y = GetThreadGroups(resolutionY, groupSizeY);
            threads.Z = GetThreadGroups(resolutionZ, groupSizeZ);

            return threads;
        }

    private:
        static uint32_t GetThreadGroups(const uint32_t resolution, const uint32_t groupSize)
        {
            if (groupSize == 0)
                return 0;

            return (resolution + groupSize - 1) / groupSize;
        }
    };
} // namespace Beer::Rendering
