include(FetchContent)

macro(LinkASSIMP TARGET ACCESS)
    # 1. Déclarer
    FetchContent_Declare(
            assimp
            GIT_REPOSITORY https://github.com/assimp/assimp
            GIT_TAG v6.0.0 # v6.0.4 est excellent
    )

    set(ASSIMP_BUILD_ASSIMP_TOOLS   OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_TESTS          OFF CACHE BOOL "" FORCE)
    set(ASSIMP_INSTALL              OFF CACHE BOOL "" FORCE)

    FetchContent_GetProperties(assimp)

    if (NOT assimp_POPULATED)
        FetchContent_MakeAvailable(assimp)

        if(TARGET IrrXML)
            set_target_properties(IrrXML PROPERTIES FOLDER ${PROJECT_NAME}/thirdparty)
        endif()
        if(TARGET assimp)
            set_target_properties(assimp PROPERTIES FOLDER ${PROJECT_NAME}/thirdparty)
        endif()
    endif()

    target_link_libraries(${TARGET} ${ACCESS} assimp)
endmacro()