#pragma once

#include "Rendering/Mesh/MeshBufferType.hpp"
#include "Vendor/magic_enum/magic_enum.hpp"
#include <print>

namespace Beer::Rendering
{
    struct MeshBufferOrder
    {
    private:
        std::vector<MeshBufferType> order;

    public:
        void AddToOrder(MeshBufferType type)
        {
            order.push_back(type);
        }

        void Clear()
        {
            order.clear();
        }

        void Print()
        {
            for (auto& element : order)
            {
                std::println("Vertex Input Buffer: {}", magic_enum::enum_name(element));
            }
        }

        const MeshBufferType GetElement(const int i) const
        {
            return order[i];
        }

        const int GetSize() const
        {
            return order.size();
        }
    };
} // namespace Beer::Rendering
