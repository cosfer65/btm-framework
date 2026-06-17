#pragma once

#include "aabb.h"

namespace btm {

    template <typename T>
    struct BVHNode {
        AABB<T> bounds;
        int  leftChild{ -1 };
        int  rightChild{ -1 };
        int  start{ 0 };  // index into primitive array
        int  count{ 0 };  // number of primitives

        bool isLeaf() const {
            return count > 0;
        }
    };

} // namespace btm
