#pragma once

#include <cstdint>

namespace btm {
    using GenericHandle = uint32_t;
    // Define specific handle types for different resources
    using BufferHandle = GenericHandle;
    using VertexArrayHandle = GenericHandle;
    using ShaderHandle = GenericHandle;  // shaders are defined in btm namespace, but we can use the same handle type here for consistency
}
