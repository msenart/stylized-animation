include(FetchContent)

macro(LinkImGui TARGET ACCESS)
    FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG v1.91.9
    )

    FetchContent_GetProperties(imgui)

    if (NOT imgui_POPULATED)
        FetchContent_MakeAvailable(imgui)
    endif()

    # ImGui has no CMakeLists.txt, so we build a static lib manually
    if (NOT TARGET imgui)
        add_library(imgui STATIC
            ${imgui_SOURCE_DIR}/imgui.cpp
            ${imgui_SOURCE_DIR}/imgui_draw.cpp
            ${imgui_SOURCE_DIR}/imgui_tables.cpp
            ${imgui_SOURCE_DIR}/imgui_widgets.cpp
            ${imgui_SOURCE_DIR}/imgui_demo.cpp
            ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
            ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        )

        target_include_directories(imgui PUBLIC
            ${imgui_SOURCE_DIR}
            ${imgui_SOURCE_DIR}/backends
        )

        # Backends require GLFW and OpenGL headers
        target_link_libraries(imgui PUBLIC glfw OpenGL::GL)

        set_target_properties(imgui PROPERTIES FOLDER ${PROJECT_NAME}/thirdparty)
    endif()

    target_link_libraries(${TARGET} ${ACCESS} imgui)
endmacro()