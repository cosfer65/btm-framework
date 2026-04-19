cmake_minimum_required(VERSION 3.20)
project(btm-framework 
    LANGUAGES C CXX
)

# Library target
add_library(btm-framework)

file(GLOB SRC_FILES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.c"
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.h"
    "${CMAKE_SOURCE_DIR}/include/*.h"
    "${CMAKE_SOURCE_DIR}/include/gl/*.h"
    "${CMAKE_SOURCE_DIR}/include/math/*.h"
    "${CMAKE_SOURCE_DIR}/include/utils/*.h"
)

target_sources(btm-framework
    PRIVATE
        ${SRC_FILES}
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


#collect examples
file(GLOB SUBDIRS
    LIST_DIRECTORIES true
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/*"
)

foreach(example ${SUBDIRS})
    if (IS_DIRECTORY ${example})
        # message ("------>Processing example: ${example}")
        get_filename_component(example_name ${example} NAME)
        # message ("------>------>Processing example: ${example_name}")
        # add example as executable
        file(GLOB example_sources CONFIGURE_DEPENDS
            "${example}/*.cpp"
        )
        add_executable(${example_name} ${example_sources})
        target_link_libraries(${example_name} PRIVATE btm-framework)
    endif()
endforeach()

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

