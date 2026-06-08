include(FetchContent)

macro(LinkCXXOPTS TARGET ACCESS)
    FetchContent_Declare(
            cxxopts
            GIT_REPOSITORY https://github.com/jarro2783/cxxopts
            GIT_TAG v3.2.1
    )

    FetchContent_GetProperties(cxxopts)

    if (NOT cxxopts_POPULATED)
        FetchContent_MakeAvailable(cxxopts)
    endif()

    target_include_directories(${TARGET} ${ACCESS} ${cxxopts_SOURCE_DIR}/include)
endmacro()