cmake_minimum_required(VERSION 3.20)

set(PROJECT_NAME "btm-framework")

project(${PROJECT_NAME}
    LANGUAGES C CXX
)

# Library target
add_library(${PROJECT_NAME}
    STATIC
    # Sources will be added later
)

include(cmake/collect_framework_sources.cmake)
collect_frm_sources(SRC_FILES)

target_sources(${PROJECT_NAME} PRIVATE ${SRC_FILES})

# Public headers
set(PUBLIC_H)
list(APPEND PUBLIC_H ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB SUBDIRS
    LIST_DIRECTORIES true
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*"
)

foreach(subdir ${SUBDIRS})
    if (IS_DIRECTORY ${subdir})
        get_filename_component(dir_name ${subdir} NAME)
        list(APPEND PUBLIC_H "${CMAKE_CURRENT_SOURCE_DIR}/include/${dir_name}")
    endif()
endforeach()

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${PUBLIC_H}>
        $<INSTALL_INTERFACE:include>
)

# C++ standard
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
# C standard
target_compile_features(${PROJECT_NAME} PUBLIC c_std_11)

# required definitions
add_definitions(-DGLEW_STATIC)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-DWIN32_LEAN_AND_MEAN)

# Platform specifics (Windows)
if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
endif()

add_library(btm::framework ALIAS ${PROJECT_NAME})

include(cmake/collect_tutorials.cmake)
collect_tutorials(tutorials ${PROJECT_NAME})

# -----------------------------
# Install rules (future-proof)
# -----------------------------
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)
