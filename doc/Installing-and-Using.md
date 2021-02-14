# Installing and Using

## Requirements

Since this is just a toy project, I am not going to put too much effort on compatibility. But it should work well under following requirements:

- C++20
- x86-64
- GCC >= 10.2 or Clang >= 11.0
- CMake >= 3.19
- Google Test >= 1.10 (optional)

MSVC is not tested but it should work well if it supports enough C++20 features. These is no compiler-specific code.

## Disable Exceptions and RTTI

This library is compatible with `-fno-exceptions` and `-fno-rtti` without loss of any feature. You can feel free to disable them.

## Install

TODO
