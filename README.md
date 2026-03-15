# GetBounds

A C++ plugin for Foundry's Nuke that automatically determines the true bounding box of an image in real-time. GetBounds scans pixel data across selected channels and crops the bounding box to fit only the non-zero region — useful for optimising downstream operations by eliminating unnecessary empty space.

Originally created by [Michael Millspaugh](http://www.tks-designs.com). Compiled and redistributed for modern Nuke versions by [Peter Mercell](https://petermercell.com).

## How It Works

GetBounds hooks into Nuke's validation stage to read the full input frame and find the tightest axis-aligned bounding box that contains all non-zero pixels. It walks each row left-to-right and right-to-left across the selected channels to determine the horizontal and vertical extents, then sets the output bounding box accordingly.

> **Note:** Because GetBounds forces a full-frame render during validation, it is designed to be placed **directly after a Read node**. Placing it further down a heavy node tree may cause the UI to lock up while the upstream nodes are forced to render.

## Knobs

| Knob | Description |
|---|---|
| **channels** | Which channels to scan for non-zero pixels |
| **black outside** | Clamp pixels outside the computed bbox to black |
| **ignore <0** | Ignore negative pixel values when computing bounds |
| **extra** | Pixel padding added around the detected bounding box |

## Installation

Pre-compiled plugins are available in the `LINUX/`, `MAC/`, and `WIN/` folders for supported Nuke versions.

1. Download the appropriate file for your platform and Nuke version:
   - Linux: `LINUX/<version>/GetBounds.so`
   - macOS: `MAC/<version>/GetBounds.dylib`
   - Windows: `WIN/<version>/GetBounds.dll`
2. Copy the file to your `~/.nuke/` directory or any path in your `NUKE_PATH`.
3. In Nuke, create the node via the tab menu by typing `GetBounds`.

### Supported Nuke Versions

| Version | Linux | macOS | Windows |
|---|:---:|:---:|:---:|
| 14.1v8 | ✓ | — | ✓ |
| 15.0v8 | ✓ | ✓ | ✓ |
| 15.1v10 | ✓ | ✓ | ✓ |
| 15.2 | ✓ | ✓ | ✓ |
| 16.0v6 | ✓ | ✓ | ✓ |
| 16.1v1 | ✓ | ✓ | ✓ |
| 17.0v1 | ✓ | ✓ | ✓ |

## Building from Source

GetBounds uses CMake with platform-specific configuration files. The source is in `src/`.

### Linux

```bash
mkdir build && cd build
cp ../CMakeLists_LINUX.txt CMakeLists.txt
cmake . -DNUKE_VERSION=17.0v1 -DCMAKE_INSTALL_PREFIX=~/.nuke
make && make install
```

### macOS

```bash
mkdir build && cd build
cp ../CMakeLists_MAC.txt CMakeLists.txt
cmake . -DNUKE_VERSION=17.0v1 -DCMAKE_INSTALL_PREFIX=~/.nuke
make && make install
```

### Windows

```cmd
mkdir build && cd build
copy ..\CMakeLists_WIN.txt CMakeLists.txt
cmake . -G "Visual Studio 17 2022" -A x64 -DNUKE_VERSION=17.0v1 -DCMAKE_INSTALL_PREFIX=%USERPROFILE%\.nuke
cmake --build . --config Release
cmake --install .
```

Set `-DNUKE_VERSION` to match your installed Nuke version. Nuke is expected at `/opt/Nuke<version>` on Linux, `/Applications/Nuke<version>` on macOS, or `C:\Program Files\Nuke<version>` on Windows (see the respective CMakeLists file for details).

## Repository Structure

```
├── src/
│   ├── GetBounds.cpp      # Plugin source
│   └── Bounds.h           # Plugin header
├── LINUX/                 # Pre-compiled .so files
├── MAC/                   # Pre-compiled .dylib files
├── WIN/                   # Pre-compiled .dll files
├── CMakeLists_LINUX.txt   # Linux build configuration
├── CMakeLists_MAC.txt     # macOS build configuration
├── CMakeLists_WIN.txt     # Windows build configuration
├── GetBounds.nk           # Example Nuke script
└── README.md
```

## Credits

- **Original author:** [Michael Millspaugh](http://www.tks-designs.com)
- **Compiled for modern Nuke versions by:** [Peter Mercell](https://petermercell.com)
