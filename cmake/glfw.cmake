#generate imported target for glfw
set(EXTERNAL_LIB_DIR "${CMAKE_SOURCE_DIR}/external")
find_library(GLFW3_LIBRARY_DEBUG
    NAMES glfw3.lib
    PATHS "${EXTERNAL_LIB_DIR}/glfw/lib/debug"
    NO_DEFAULT_PATH
)
message(STATUS "Found GLFW library: ${GLFW3_LIBRARY_DEBUG}")
add_library(glfw_d STATIC IMPORTED)

find_library(GLFW3_LIBRARY
    NAMES glfw3.lib
    PATHS "${EXTERNAL_LIB_DIR}/glfw/lib/release"
    NO_DEFAULT_PATH
)
message(STATUS "Found GLFW library: ${GLFW3_LIBRARY}")
add_library(glfw STATIC IMPORTED)

set(EXTERNAL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/external/glfw/include")
set_target_properties(glfw PROPERTIES
    IMPORTED_LOCATION "${GLFW3_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_INCLUDE_DIR}"
)
set_target_properties(glfw_d PROPERTIES
    IMPORTED_LOCATION "${GLFW3_LIBRARY_DEBUG}"
    INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_INCLUDE_DIR}"
)
