#pragma once

namespace Beer::System
{
    class IEntityManager
    {
    public:
        virtual ~IEntityManager() = default;
        virtual void Update() = 0;
    };
} // namespace Beer::System
