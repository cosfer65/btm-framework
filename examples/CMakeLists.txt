#collect tutorials directories
file(GLOB SUBDIRS
    LIST_DIRECTORIES true
    "${CMAKE_CURRENT_SOURCE_DIR}/${TUTORIALS_DIRECTORY}/*"
)

foreach(tutorial ${SUBDIRS})
    if (IS_DIRECTORY ${tutorial})
        # strip directory path to get tutorial name
        get_filename_component(tutorial_name ${tutorial} NAME)
        add_subdirectory(${tutorial_name})
    endif()
endforeach()
