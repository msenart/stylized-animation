include(FetchContent)

macro(LinkGLFW TARGET ACCESS)
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG 3.3.10
    )

    FetchContent_GetProperties(glfw)

    if (NOT glfw_POPULATED)
        FetchContent_MakeAvailable(glfw)

        # Just configure GLFW only
        set(GLFW_BUILD_EXAMPLES     OFF CACHE BOOL "Build Examples" FORCE)
        set(GLFW_BUILD_TESTS        OFF CACHE BOOL "Build tests" FORCE)
        set(GLFW_BUILD_DOCS         OFF CACHE BOOL "Build docs" FORCE)
        set(GLFW_INSTALL            OFF CACHE BOOL "Configure an install" FORCE)

        # Set the target's folders
        if (TARGET glfw)
            set_target_properties(glfw PROPERTIES FOLDER ${PROJECT_NAME}/thirdparty)
        endif()
    endif()

#    target_include_directories(${TARGET} ${ACCESS} ${glfw_SOURCE_DIR}/include)
    target_link_libraries(${TARGET} ${ACCESS} glfw)

#    add_dependencies(${TARGET} glfw)
endmacro()