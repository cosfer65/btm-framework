cmake_minimum_required(VERSION 3.20)
project(btm-framework 
    LANGUAGES C CXX
)

# Library target
add_library(btm-framework)

target_sources(btm-framework
    PRIVATE
        src/glew.c
        src/app_core.cpp
        src/app_core.h
        src/common_dialogs.cpp
        src/core_application.cpp
        src/gl_font.cpp
        src/gl_math.cpp
        src/gl_mesh.cpp
        src/gl_prim.cpp
        src/gl_shaders.cpp
        src/gl_timer.cpp
        src/gl_window.cpp
)

# Public headers
target_include_directories(btm-framework
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/gl>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/math>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/utils>
        $<INSTALL_INTERFACE:include>
)
# C++ standard
target_compile_features(btm-framework PUBLIC cxx_std_20)
# C standard
target_compile_features(btm-framework PUBLIC c_std_11)

# Platform specifics (Windows)
if (WIN32)
    target_link_libraries(btm-framework PRIVATE opengl32)
endif()

add_library(btm::framework ALIAS btm-framework)

# -----------------------------
# Options
# -----------------------------
option(BTM_BUILD_EXAMPLES "Build example programs" ON)

# -----------------------------
# Examples
# -----------------------------
if(BTM_BUILD_EXAMPLES)
	add_subdirectory(examples)
    #add_executable(btm-example examples/minimal_window.cpp)
    #target_link_libraries(btm-example PRIVATE btm::framework)
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

