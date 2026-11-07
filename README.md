# edaSymbols (`eda-symbols`)

[![Standard](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B20)
[![Platform](https://img.shields.io/badge/Platform-Windows%20(Direct2D)-0078D6.svg)](https://docs.microsoft.com/en-us/windows/win32/direct2d/direct2d-portal)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

C++20 Direct2D library containing IEEE and IEC schematic symbols for circuit CAD and EDA

## Structure

```
eda-symbols/
├── include/
│   └── eda/
│       └── symbols/
│           ├── types.hpp              # shared enums and anchor structs
│           ├── ieee.hpp
│           └── iec.hpp
├── src/
│   ├── ieee.cpp
│   └── iec.cpp
├── tests/
│   ├── CMakeLists.txt
│   ├── test_symbols.cpp               # geometry bounds and sink tests
│   └── export_symbol_images.cpp       # PNG exporter for eda-symbols
├── .gitignore
├── CMakeLists.txt
├── LICENSE
└── README.md
```

## Requirements

- **Operating System**: Windows 10 / 11 (Direct2D 1.0 or later)
- **Compiler**: MSVC 19.29+ (Visual Studio 2019 v16.11+) or Clang-CL supporting C++20
- **Build System**: CMake 3.20+
- **SDK**: Windows SDK 10.0.18362.0 or higher

## Integration

Add the following to `CMakeLists.txt`:

```cmake
include(FetchContent)

FetchContent_Declare(
    eda_symbols
    GIT_REPOSITORY https://github.com/haievyi/eda-symbols.git
    GIT_TAG        v0.1.0
)
FetchContent_MakeAvailable(eda_symbols)

target_link_libraries(${PROJECT_NAME} PRIVATE eda::symbols)
```
