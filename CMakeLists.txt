cmake_minimum_required(VERSION 3.20)
project(btm-framework LANGUAGES CXX)

# -----------------------------
# Global configuration
# -----------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# -----------------------------
# Library target
# -----------------------------
add_library(btm-framework)

target_sources(btm-framework
    PRIVATE
        src/core.cpp        # add more as needed
)

target_include_directories(btm-framework
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_library(btm::framework ALIAS btm-framework)

# -----------------------------
# Options
# -----------------------------
option(BTM_BUILD_EXAMPLES "Build example programs" ON)

# -----------------------------
# Examples
# -----------------------------
if(BTM_BUILD_EXAMPLES)
    add_executable(btm-example examples/minimal_window.cpp)
    target_link_libraries(btm-example PRIVATE btm::framework)
endif()

# -----------------------------
# Install rules (future-proof)
# -----------------------------
install(TARGETS btm-framework
    EXPORT btm-framework-targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)

install(EXPORT btm-framework-targets
    FILE btm-framework-config.cmake
    NAMESPACE btm::
    DESTINATION lib/cmake/btm-framework
)

