#!/usr/bin/env bash
set -e

# Set project root as current directory
project_root=$(pwd)

# Create and enter host build directory
build_dir="$project_root/build-host"
mkdir -p "$build_dir"
cd "$build_dir"

# Run CMake for host compiler, export compile commands
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON "$project_root"

# Run clang-tidy on all source and header files under src and include
# Adjust this find if you want to lint other folders/files
find "$project_root/src" "$project_root/include" -type f \( -name "*.cpp" -o -name "*.hpp" \) | \
  xargs clang-tidy -p "$build_dir" --quiet

echo "clang-tidy analysis complete."
