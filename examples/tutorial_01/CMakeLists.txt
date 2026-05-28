set(NAME "tutorial_01")
add_executable(${NAME} main.cpp)
target_link_libraries(${NAME} PRIVATE btm-framework)
if(MSVC)
    target_link_options(${NAME} PRIVATE "/SUBSYSTEM:WINDOWS")
else()
    target_link_options(${NAME} PRIVATE "-mwindows")
endif()
set_target_properties(${NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/runtime"
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/runtime"
)