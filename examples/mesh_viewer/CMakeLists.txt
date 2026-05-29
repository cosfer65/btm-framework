set(NAME "mesh_viewer")

# colect source files in src directory and its subdirectories
set (LOCAL_FILES)
file(GLOB LOCAL_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable(${NAME} ${LOCAL_FILES})

target_link_libraries(${NAME} PRIVATE btm-framework)

if(MSVC)
    target_link_options(${NAME} PRIVATE "/SUBSYSTEM:WINDOWS")
else()
    target_link_options(${NAME} PRIVATE "-mwindows")
endif()

set_target_properties(${NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/runtime"
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/runtime"
)