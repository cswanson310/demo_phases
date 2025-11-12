# toy_cpp

Small C++17 toy project built with Bazel and GoogleTest.

## Prerequisites (macOS, zsh)

- Xcode command line tools: `xcode-select --install`
- Homebrew (optional but recommended): https://brew.sh
- Bazelisk (recommended) or Bazel: `brew install bazelisk`

## Quick Start (Bazel)

```bash
cd /Users/charlie.swanson/toy_cpp

# Build everything
bazel build //...

# Run the application
bazel run //:toy_app

# Run tests
bazel test //...

# Run tests with verbose output
bazel test //... --test_output=all
```

## Available Targets

- `//:toy_lib` - The core library with the `add` function
- `//:toy_app` - The main application binary
- `//:unit_tests` - GoogleTest unit tests

## Development Tips

- Use `bazel clean` to clean build artifacts
- Use `bazel query //...` to see all available targets
- Build output is in `bazel-bin/`

---

## Legacy CMake Build (Deprecated)

<details>
<summary>Click to expand CMake instructions</summary>

Prerequisites:
- CMake: `brew install cmake`

Build steps:
```bash
cd /Users/charlie.swanson/toy_cpp
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
ctest --output-on-failure
```
</details>
