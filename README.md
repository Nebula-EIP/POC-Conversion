# Nebula POC Conversion

Prototype C++17 for the three conversion architectures in the supplied study:

- `nebula_poc_a`: templates plus a deliberately limited C parser;
- `nebula_poc_b`: isolated Clang/LibTooling integration point;
- `nebula_poc_c`: typed shared IR with validation and C generation;
- `nebula_harness`: common runner and measurement output;
- `corpus`: C11 cases T01-T13;
- `tests`: smoke checks for generation, round trip, validation, and refusal.

## Prerequisites

- CMake 3.21 or newer;
- C++17 compiler (MSVC 2022, GCC 11+, or Clang 17+);
- LLVM/Clang 17+ with CMake package files for the real POC B importer.

LLVM is not vendored. Configure `CMAKE_PREFIX_PATH` to the matching LLVM installation; all Clang libraries must come from the same version.

## Build

```powershell
cmake -S . -B build -G Ninja -DNEBULA_ENABLE_CLANG=OFF
cmake --build build
ctest --test-dir build --output-on-failure
```

With LLVM installed:

```powershell
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=C:/LLVM -DNEBULA_ENABLE_CLANG=ON
cmake --build build
```

Without LLVM, POC B still compiles and returns `B_CLANG_UNAVAILABLE` instead of silently reporting a successful import.

## Scope

The common model and generator are implemented first. POC A demonstrates the T01/T02/T06-shaped path and explicit rejection. Its parser is intentionally small and must be expanded for the complete T01-T13 acceptance gate. POC B's production `RecursiveASTVisitor` is the next integration step once a pinned LLVM development package is installed. Measurements must be run in the target environment and are not fabricated here.