# Lunatic Vibes

Lunatic Vibes is a rhythm game that plays community-made charts in BMS format.

The project is basically a rewritten version of the Lunatic Rave 2 (beta3 100201). Some useful functions will be added in the future as well.

The project is still in development stage. Please do not expect a bug-free experience. Feel free to open issues.

---------------

## Quick Start

* Download the latest release
* Copy LR2files folder from LR2 (must include default theme; a fresh copy right from LR2 release is recommended)

**Do NOT use this application to load unauthorized copyrighted contents (e.g. charts, skins).**

---------------

## Build

### Windows (CMake + Visual Studio 2022)

1. Clone repository
    ```
    git clone https://github.com/yaasdf/lunaticvibes.git
    cd lunaticvibes
    git submodule update --init --recursive
    ```

2. Configure / Build FFmpeg (Optional)

    The configured header files are included now (see [#1](https://github.com/yaasdf/lunaticvibes/issues/1)), though it's recommended to configure FFmpeg by yourself.

    If you choose to configure FFmpeg, check out `ext/FFmpeg/build_lib.md` for instructions. Default configuration emits chunkload around 100MB


3. Install dependencies via vcpkg (skip any you have already installed)
    ```
    vcpkg install openssl:x64-windows
    vcpkg install boost:x64-windows
    ```

4. Choose a generator to build

    a. VS2022 (via console)

        ```
        mkdir build
        cd build
        cmake .. -G "Visual Studio 17 2022" -A x64
        cmake --build . --config Release -j -t LunaticVibes
        ```

    b. Ninja (via IDE)

        - Open project folder with Visual Studio
        - Open CMake configuration, fill CMake toolchain file to the one from vcpkg like "D:/vcpkg/scripts/buildsystems/vcpkg.cmake"
        - Build LunaticVibes.exe

---------------

## License
* MIT License