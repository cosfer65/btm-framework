function (collect_frm_sources SRC_FILES)

    set (LOCAL_FILES)
    # colect source files in src directory and its subdirectories
    file(GLOB LOCAL_FILES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/src/*.c"
        "${CMAKE_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_SOURCE_DIR}/src/*.h"
    )

    file(GLOB SUBDIRS
        LIST_DIRECTORIES true
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*"
    )

    foreach(subdir ${SUBDIRS})
        if (IS_DIRECTORY ${subdir})
            file(GLOB SUBDIR_FILES CONFIGURE_DEPENDS
                "${subdir}/*.c"
                "${subdir}/*.cpp"
                "${subdir}/*.h"
            )
            list(APPEND LOCAL_FILES ${SUBDIR_FILES})
        endif()
    endforeach()

    # colect header files in include directory and its subdirectories
    file(GLOB LOCAL_HEADERS CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/include/*.h"
    )
    list(APPEND LOCAL_FILES ${LOCAL_HEADERS})

    file(GLOB SUBDIRS
        LIST_DIRECTORIES true
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*"
    )

    foreach(subdir ${SUBDIRS})
        if (IS_DIRECTORY ${subdir})
            file(GLOB SUBDIR_FILES CONFIGURE_DEPENDS
                "${subdir}/*.h"
            )
            list(APPEND LOCAL_FILES ${SUBDIR_FILES})
        endif()
    endforeach()

    # message (STATUS "Collected framework source files:")
    # foreach(tutorial ${LOCAL_FILES})
    #     message (STATUS "Found source file: ${tutorial}")
    # endforeach()

    set(${SRC_FILES} "${LOCAL_FILES}" PARENT_SCOPE) # Pass value back
endfunction()