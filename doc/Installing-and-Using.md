# Installing and Using

- [Requirements](#requirements)
- [Disable Exceptions and RTTI](#disable-exceptions-and-rtti)
- [Install](#install)

## Requirements

Since this is just a toy project, I am not going to put too much effort on
compatibility. But it should work well under following requirements:

- C++20
- GCC >= 10.2 or Clang >= 11.0
- CMake >= 3.19
- Google Test >= 1.10 (optional)

MSVC is not tested but it should work well if it supports enough C++20
features.

## Disable Exceptions and RTTI

This library is compatible with `-fno-exceptions` and `-fno-rtti` without
loss of any feature. You can feel free to disable them.

## Install

Currently this library only supports manual installation.

Include this repo to your project by git submodule or whatever you like,
then add this to your `CMakeLists.txt`:

```cmake
add_subdirectory(path/to/qcpc-parser-comb-v2)
```

This exports the `qcpc2` target which can be linked against any other target.

```cmake
add_executable(myexe mysources...)
target_link_libraries(myexe PRIVATE qcpc2)
```
