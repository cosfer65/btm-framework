# btm-framework

A minimal, modern C++20 framework providing the foundational building blocks for
the **BeforeTheMesh** educational platform and **TheMeshProject** engineering codebase.

The framework offers a clean, modular architecture for graphics, math utilities,
windowing, and future geometry/CAE components. It is designed to be lightweight,
readable, and suitable for progressive tutorial development.

---

## Features

- Modern C++20, target-based CMake
- Header-first design where appropriate
- Clear separation of `include/` and `src/`
- Optional examples demonstrating minimal window creation and rendering
- Stable API intended for long-term educational use

---

## Repository Structure

btm-framework/
include/        # Public headers
src/            # Library implementation
examples/       # Optional example programs
cmake/          # CMake modules (future)
CMakeLists.txt

---

## Building

## bash
mkdir build
cd build
cmake ..
cmake --build .

---

## To build examples
cmake -DBTM_BUILD_EXAMPLES=ON ..
