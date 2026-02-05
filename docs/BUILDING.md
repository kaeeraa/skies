# How to build `skies`?

## Prerequisites

- `cmake` >= 4.1.2 (the project enables CMake 4.1.2-specific features and exports compile commands)
- `clang` >= 19.1
- `boost` >= 1.89
- `ninja` >= 1.13
- `protobuf` (compiler + runtime)
- `absl` libraries (strings, status, flat hash map, log, etc.)
- Optional but recommended: `mold` as the linker and `ccache` as the compiler launcher (already wired into `CMakeLists.txt` if found on your `PATH`).

## Recommended workflow (Nix + `devenv`)

1. Install `devenv` and `direnv`, then run `direnv allow` so the project can inject its environment.
2. `use devenv` from the repository root (⌚ this is what `.envrc` already does).
3. Execute the build task that already wraps the production CMake command:

   ```bash
   devenv task skies:build
   ```

   That command sets Clang as the compiler, enables `CMAKE_EXPORT_COMPILE_COMMANDS`, and generates a Ninja build in `./build`.
4. Run the binary with:

   ```bash
   devenv task skies:run
   ```

   This runs `./build/skies`; the task reuses the build step automatically so you can iterate quickly.

## Manual CMake build (non-Nix)

If you prefer a vanilla CMake flow, call the standard configure/build steps. Make sure you set Clang if it is not your default compiler:

```bash
cmake -S . -B build -G Ninja \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++
cmake --build build
./build/skies
```

Use `cmake --build build --target <target>` or `ninja -C build <target>` to rebuild a specific target or re-run the tests once they exist.

### Notes

- `CMakeLists.txt` already looks for `Boost`, `absl`, and the `protobuf` libraries and headers, so make sure they are discoverable (e.g., installed via your package manager or `devenv`).
- `Mold` and `ccache` are optional: if they are in your `PATH`, the build prints `Using mold as linker` / `Using ccache` during the configure step.
- If you customize the build directory, keep `CMAKE_EXPORT_COMPILE_COMMANDS` enabled so editors and linters can consume the resulting `compile_commands.json`.
