<div align="center">

# Stylized Animation

An OpenGL engine exploring three classic techniques for giving 3D animation a hand-drawn, non-photorealistic look.

![C++](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=cplusplus&logoColor=white)
![OpenGL](https://img.shields.io/badge/OpenGL-4.6-5586A4?logo=opengl&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-%E2%89%A5%203.11-064F8C?logo=cmake&logoColor=white)
![License](https://img.shields.io/badge/license-Apache%202.0-blue)

<video src="./assets/demo.mp4" width="720" controls autoplay muted loop></video>

</div>

## Overview

This is a computer graphics end-of-year project (IGR — Image, Graphics, Realtime) built from scratch in C++ and OpenGL 4.6. Rather than aiming for photorealism, the engine explores three complementary techniques used in stylized/cartoon rendering, each implemented and studied by one team member:

| Topic | Description | Author |
| --- | --- | --- |
| **Non-Photorealistic Rendering (NPR)** | Toon/cel shading with quantized diffuse bands, halftone specular highlights, cross-hatched shadows and rim lighting | **Mathieu Senart** |
| **Contour drawing** | Screen-space edge detection driven by depth/normal discontinuities and mesh-id buffers, to outline silhouettes and creases | **Alice Jeannin** |
| **Squash & stretch / speed effect** | Vertex smearing along each bone's recent motion trajectory ("SMEAR" technique) to exaggerate fast motion | **Francisco De Castro Leal Henriques** |

A [technical report](Stylized-animation-report.pdf) and the [presentation slides](./Final%20presentation%20slides.pdf) go into more depth on the theory and implementation choices behind each technique. The papers that directly inspired this project are listed in [`docs/`](./docs) and referenced below.

## Features

- **Multiple render targets (MRT) pipeline** — a single "hybrid" pass writes scene color, per-mesh IDs and view-space normals/depth to three textures at once, which downstream passes consume for contours and mesh picking.
- **Toon shading** with configurable diffuse/specular/ambient/rim terms and a Phong or GGX BSDF backend.
- **Screen-space contour detection** from depth and normal discontinuities.
- **SMEAR-based squash & stretch** — per-bone motion deltas are baked offline (and cached in `assets/deltas/`) then splined per-vertex at draw time for cartoon-style speed lines and follow-through.
- **Optional Kuwahara anisotropic filter** post-process for a painterly look, toggled live from the UI.
- **Live shader hot-reloading** and an in-app shader/status inspector.
- **Click-to-select** meshes in the viewport to inspect and swap their shaders or animation at runtime.
- **Headless video capture** via FFMPEG, with a fixed simulated framerate for deterministic renders.

## Getting started

### Prerequisites

- A C++17 compiler and [CMake](https://cmake.org/) ≥ 3.11
- A GPU/driver supporting OpenGL 4.6
- [FFMPEG](https://ffmpeg.org/) on your `PATH` — optional, only needed for video capture

The CMake configuration itself has no OS-specific logic (all dependencies — GLFW, GLAD, GLM, Assimp, ImGui, cxxopts, stb — are fetched and built automatically by CMake). The application code does, however, rely on POSIX APIs (`unistd.h`, `popen`/`pclose` for the FFMPEG pipe), so it builds out of the box on Linux, macOS, and on Windows through a POSIX-compatible toolchain such as MinGW-w64/MSYS2 or WSL. Native MSVC would need `_popen`/`_pclose` substitutions.

### Build & run

```sh
cmake -B build
cmake --build build
cd build && make && cd ..
./stylize-animation
```

(`make_and_run.sh` wraps the same steps for a quick debug build.)

Shaders and assets are loaded relative to the working directory, and the built binary is copied to the repository root as part of the build — always run `./stylize-animation` from the repository root, as shown above.

## Usage

### Command-line options

| Flag | Description |
| --- | --- |
| `-s`, `--save-video` | Pipe every rendered frame to FFMPEG and save it as `output.mp4` |
| `-H`, `--headless` | Don't open a window; useful combined with `--save-video` to render offscreen |
| `-f`, `--frames <n>` | Stop after generating `n` frames (default: run indefinitely) |
| `-t`, `--target-fps <n>` | Fix the simulated timestep to `n` fps instead of wall-clock delta time, for deterministic/frame-accurate captures |
| `-h`, `--help` | Show usage |

Render 300 frames at a fixed 60 fps, headless, into `output.mp4`:

```sh
./stylize-animation --headless --save-video --frames 300 --target-fps 60
```

### Controls

| Input | Action |
| --- | --- |
| **F1** | Toggle camera control (captures the mouse for looking around) |
| Mouse look + **WASD** | Move the camera while camera control is enabled (**Space** / **Left Shift** for up/down) |
| **F2** | Reload all shaders from disk |
| **F3** | Pause / resume the animation |
| **F4** | Hide / show the UI |
| Left click on a mesh | Select it and inspect/swap its shaders or animation |

While the UI is visible, three ImGui windows are available:

- **Console** — filterable log output (Info / Warn / Error, with search)
- **Render Stats** — FPS, draw calls, object/light/mesh counts, camera info, and the Kuwahara filter toggle
- **Shaders** — every loaded shader pipeline with its compile status, and a one-click reload-all

## Project structure

```
src/
├── core/       window, logging, asset management
├── scene/      camera, transforms, lights, cinematic timeline
├── renderer/   render pipeline/passes, meshes (static/animated/smear), shader management
└── ui/         ImGui panels (console, stats, selection)
shaders/        GLSL sources for the hybrid, KAF and final render passes
docs/           background papers that motivated each technique
assets/         meshes, textures and cached SMEAR motion deltas
```

## References

The following papers guided the implementation (full PDFs in [`docs/`](./docs)):

- Toon shading & stylized patterns — *Dynamic 2D Patterns for Shading 3D Scenes* (2007), *Apparent Relief: a Shape Descriptor for Stylized Shading* (2008)
- Contours — *Implicit Brushes for Stylized Line-based Rendering* (2011)
- Squash & stretch / speed effect — *SMEAR: Stylized Motion Exaggeration with ARt-direction* (2024), *Trajectory-aware Smears for Stylized 3D Animations* (2025)

A [tutorial on skeletal animation with Assimp](https://www.youtube.com/watch?v=r6Yv_mh79PI) was also used as a starting point for the animation pipeline.

## Acknowledgments

Test/demo character rigs and animations under `assets/meshes/` (Mixamo-style FBX rigs, the "Tai Lung" model, and the Stanford Bunny) are third-party assets included for research and demonstration purposes only.

## License

This project is licensed under the [Apache License 2.0](./LICENSE).
