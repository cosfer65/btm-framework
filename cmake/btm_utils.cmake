# ======================================================================
#  btm_utils.cmake
#
#  Shared helper functions for the BeforeTheMesh ecosystem.
#  These utilities are intentionally minimal, modern, and target-based.
#
#  Usage:
#      include(cmake/btm_utils.cmake)
#
# ======================================================================

include_guard(GLOBAL)

# ----------------------------------------------------------------------
# btm_set_default_build_type(<type>)
#
# Sets a default build type for single-config generators (Ninja/Makefiles)
# without interfering with multi-config generators (Visual Studio/Xcode).
# ----------------------------------------------------------------------
function(btm_set_default_build_type default_type)
    if(NOT CMAKE_CONFIGURATION_TYPES) # single-config generator
        if(NOT CMAKE_BUILD_TYPE)
            message(STATUS "BTM: Setting default build type to '${default_type}'")
            set(CMAKE_BUILD_TYPE "${default_type}" CACHE STRING "Build type" FORCE)
        endif()
    endif()
endfunction()

# ----------------------------------------------------------------------
# btm_enable_warnings(<target>)
#
# Enables a consistent set of warnings across compilers.
# Does NOT add -Werror or break user builds.
# ----------------------------------------------------------------------
function(btm_enable_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

# ----------------------------------------------------------------------
# btm_link_configured(
#       <target>
#       DEBUG <debug_lib>
#       RELEASE <release_lib>
# )
#
# Convenience wrapper for linking different libraries per configuration.
# Example:
#   btm_link_configured(myapp
#       DEBUG   glfw3d.lib
#       RELEASE glfw3.lib
#   )
# ----------------------------------------------------------------------
function(btm_link_configured target)
    set(options)
    set(oneValueArgs DEBUG RELEASE)
    cmake_parse_arguments(BTM "${options}" "${oneValueArgs}" "" ${ARGN})

    if(BTM_DEBUG)
        target_link_libraries(${target} PRIVATE
            debug ${BTM_DEBUG}
        )
    endif()

    if(BTM_RELEASE)
        target_link_libraries(${target} PRIVATE
            optimized ${BTM_RELEASE}
        )
    endif()
endfunction()

# ----------------------------------------------------------------------
# btm_print_target(<target>)
#
# Debug helper: prints include dirs, compile defs, and linked libs.
# Useful during development.
# ----------------------------------------------------------------------
function(btm_print_target target)
    message(STATUS "----- BTM Target Info: ${target} -----")

    get_target_property(inc ${target} INCLUDE_DIRECTORIES)
    get_target_property(def ${target} COMPILE_DEFINITIONS)
    get_target_property(lib ${target} LINK_LIBRARIES)

    message(STATUS "Include dirs: ${inc}")
    message(STATUS "Definitions : ${def}")
    message(STATUS "Libraries   : ${lib}")
endfunction()
