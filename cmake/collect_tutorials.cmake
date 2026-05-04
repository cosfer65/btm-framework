function (collect_tutorials TUTORIALS_DIRECTORY FRAMEWORK_NAME)

    #collect tutorials directories
    file(GLOB SUBDIRS
        LIST_DIRECTORIES true
        "${CMAKE_CURRENT_SOURCE_DIR}/${TUTORIALS_DIRECTORY}/*"
    )

    foreach(tutorial ${SUBDIRS})
        if (IS_DIRECTORY ${tutorial})
            # add tutorial as executable
            # first collect sources
            file(GLOB tutorial_sources CONFIGURE_DEPENDS
                "${tutorial}/*.cpp"
                "${tutorial}/*.h"
            )
            # strip directory path to get tutorial name and platform prefix
            get_filename_component(tutorial_name ${tutorial} NAME)
            string(SUBSTRING ${tutorial_name} 0 4 PLATFORM)
            string(SUBSTRING ${tutorial_name} 4 -1 NAME)
            string(COMPARE EQUAL ${PLATFORM} "win_" isEqual)
            if (isEqual)
                add_executable(${NAME} WIN32 ${tutorial_sources})
                message("${NAME} is windows tutorial")
                if(MSVC)
                    target_link_options(${NAME} PRIVATE "/SUBSYSTEM:WINDOWS")
                else()
                    target_link_options(${NAME} PRIVATE "-mwindows")
                endif()
            else()
                message("${NAME} is command line tutorial")
                add_executable(${NAME} ${tutorial_sources})
            endif()
            target_link_libraries(${NAME} PRIVATE ${FRAMEWORK_NAME})

            set_target_properties(${NAME} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/runtime"
                VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/runtime"
            )
        endif()
    endforeach()
endfunction()
