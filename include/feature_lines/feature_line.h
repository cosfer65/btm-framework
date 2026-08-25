#pragma once
#include <vector>
#include "vector.h"

namespace btm{
    template<typename T>
    struct FeatureLine
    {
        // Ordered list of vertex indices forming the polyline
        std::vector<int> vertices;

        // Optional: explicit 3D points (if we want sub‑vertex precision later)
        std::vector<basepoint3<T>> points;

        // Convenience: clear both containers
        void clear()
        {
            vertices.clear();
            points.clear();
        }

        bool empty() const { return vertices.empty(); }
    };
}
