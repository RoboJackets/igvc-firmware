#!/usr/bin/env bash
# Bash script for linting all source files with clang-tidy

# Abort on first error, Pipe failure
set -e
set -o pipefail

# Additional ignore files
ignore_files=(mbed_config.h build cmake-build-debug)

# Ignore submodules
submodules=$(grep 'path = ' ../.gitmodules | cut -d= -f2-)

exclude_files=""
for file in "${ignore_files[@]}"; do
    exclude_files=$(printf "${file}\n${exclude_files}")
done

exclude_files=$(printf "${submodules}\n${exclude_files}")
exclude_flags=""
while read -r file; do
    exclude_flags="${exclude_flags} -path ../${file} -prune -o"
done <<< "$exclude_files"

# Find all files that aren't ignore files or submodules
source_files=$(find ../ ${exclude_flags} \( -name '*.h' -or -name '*.hpp' -or -name '*.cpp' \) -print)

# Run clang-tidy on all source files
clang-tidy -p ../build/compile_commands.json ${source_files}
