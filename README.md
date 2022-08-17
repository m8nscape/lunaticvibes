# Lunatic Vibes

Lunatic Vibes is a rhythm game that plays community-made charts in BMS format.

The project is basically a rewritten version of the Lunatic Rave 2 (beta3 100201). Some useful functions will be added in the future as well.

The project is still in development stage. Please do not expect a bug-free experience. Feel free to open issues.

---------------

## Quick Start

* Download the latest release
* Copy LR2files folder from LR2 (must include default theme; a fresh copy right from LR2 release is recommended)

### **Do NOT use this application to load unauthorized copyrighted contents (e.g. charts, skins).**

---------------

## Build

### Windows (Visual Studio 2022)

1. Clone repository
    ```
    git clone https://github.com/yaasdf/lunaticvibes.git
    cd lunaticvibes
    git submodule update --init --recursive
    ```

2. Configure / Build FFmpeg (this step may be removed in the future)

    Check out ext/FFmpeg/build_lib.md for instructions. Default configuration emits chunkload around 100MB

3. Install dependencies via vcpkg (skip any you have already installed)
    ```
    vcpkg install boost:x64-windows
    ```
    You may have to modify CMake Toolchain File path to somewhat like "D:/vcpkg/scripts/buildsystems/vcpkg.cmake" in VS cmake configuration.

4. build game.exe

---------------

## 
License
-
* MIT License