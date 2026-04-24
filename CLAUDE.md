# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Context

Academic graphics project from Telecom Paris IGR (Computer Graphics) implementing stylized animation techniques. Reference papers are in `resources/` — consult them for the rendering algorithms to implement.

## Build

**Requirements:** CMake 3.11+, C++17 compiler, Git, OpenGL 4.6 GPU

All dependencies (GLFW 3.3.10, GLAD 0.1.33, GLM 0.9.9.8, STB, Assimp 6.0.0, ImGui 1.91.9) are auto-downloaded via CMake FetchContent on first configure — no manual installation needed.

```bash
cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build cmake-build-debug
./cmake-build-debug/stylize-animation
```

The project is also configured for CLion (`.idea/` present); CLion can open and build it directly.

## Architecture

The project is in early skeleton phase. `src/main.cpp` is the sole source file. The planned dependency stack:

| Layer | Libraries |
|-------|-----------|
| Window / Input | GLFW |
| OpenGL loader | GLAD (OpenGL 4.6 core) |
| Math | GLM |
| Image loading | STB (`stb_image.h`) |
| 3D model import | Assimp |
| Debug UI | ImGui (GLFW + OpenGL3 backends) |

**ImGui** is built as a static library from source inside `cmake/LinkImGui.cmake` — it has no upstream CMakeLists.txt. When adding new ImGui backends or widgets, edit that file.

Optional features are toggled via CMake options (`ENABLE_ASSIMP`, `ENABLE_IMGUI`), both defaulting to ON.

## CMake Conventions

- All dependency wiring lives in `cmake/Link<Lib>.cmake` macros, called from the root `CMakeLists.txt`.
- To add a new library, create `cmake/Link<Name>.cmake` following the existing pattern and call it from `CMakeLists.txt`.
- The executable target is named `stylize-animation`.
