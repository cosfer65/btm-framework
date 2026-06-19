set(NAME "btm_unit_tests")

message(STATUS "-----> Configuring Unit Tests for btm-framework")

# colect source files in src directory and its subdirectories
set (LOCAL_FILES)
file(GLOB LOCAL_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)
add_executable(${NAME} ${LOCAL_FILES})

target_link_libraries(${NAME} PUBLIC btm-framework)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-DWIN32_LEAN_AND_MEAN)

set_target_properties(${NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/runtime"
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/runtime"
)
