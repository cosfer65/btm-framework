set(NAME "tutorial_02")
add_executable(${NAME} main.cpp)
target_link_libraries(${NAME} PRIVATE btm-framework)
set_target_properties(${NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/runtime"
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/runtime"
)