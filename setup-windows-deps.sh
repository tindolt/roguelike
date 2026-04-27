#!/usr/bin/env bash
# setup-windows-deps.sh
# Run once to prepare Windows cross-compile dependencies (SDL3 + libtcod static).
# Requires: x86_64-w64-mingw32-g++, cmake, ninja-build, git, curl
#
# Outputs:
#   lib/          - libSDL3.dll.a, libtcod.a (and any other .a from libtcod install)
#   .windows-include/ - SDL3/ and libtcod headers
#   .windows-deps/SDL3-.../x86_64-w64-mingw32/bin/SDL3.dll  <- ship alongside .exe

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DEPS_DIR="$SCRIPT_DIR/.windows-deps"
LIB_DIR="$SCRIPT_DIR/lib"
WIN_INCLUDE="$SCRIPT_DIR/.windows-include"

SDL3_VERSION="3.2.10"
LIBTCOD_TAG="2.2.2"

mkdir -p "$DEPS_DIR" "$LIB_DIR" "$WIN_INCLUDE"

# ── SDL3 (pre-built MinGW dev package from GitHub) ────────────────────────────
SDL3_ARCHIVE="SDL3-devel-${SDL3_VERSION}-mingw.tar.gz"
SDL3_URL="https://github.com/libsdl-org/SDL/releases/download/release-${SDL3_VERSION}/${SDL3_ARCHIVE}"
SDL3_MINGW="$DEPS_DIR/SDL3-${SDL3_VERSION}/x86_64-w64-mingw32"

if [[ ! -f "$DEPS_DIR/$SDL3_ARCHIVE" ]]; then
    echo "==> Downloading SDL3 ${SDL3_VERSION} MinGW dev package..."
    curl -fsSL --retry 3 -o "$DEPS_DIR/$SDL3_ARCHIVE" "$SDL3_URL"
fi

if [[ ! -d "$DEPS_DIR/SDL3-${SDL3_VERSION}" ]]; then
    echo "==> Extracting SDL3..."
    tar -xzf "$DEPS_DIR/$SDL3_ARCHIVE" -C "$DEPS_DIR"
fi

cp -f "$SDL3_MINGW/lib/libSDL3.dll.a" "$LIB_DIR/"
cp -rf "$SDL3_MINGW/include/SDL3" "$WIN_INCLUDE/"

# ── CMake MinGW toolchain file ─────────────────────────────────────────────────
TOOLCHAIN="$DEPS_DIR/mingw-toolchain.cmake"
cat > "$TOOLCHAIN" << 'TOOLCHAIN_EOF'
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER   x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER  x86_64-w64-mingw32-windres)
set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
TOOLCHAIN_EOF

# ── libtcod static (cross-compiled for Windows) ────────────────────────────────
LIBTCOD_SRC="$DEPS_DIR/libtcod"
LIBTCOD_BUILD="$DEPS_DIR/libtcod/build"
LIBTCOD_INSTALL="$DEPS_DIR/libtcod-install"

if [[ ! -d "$LIBTCOD_SRC" ]]; then
    echo "==> Cloning libtcod ${LIBTCOD_TAG}..."
    git clone --depth 1 --branch "$LIBTCOD_TAG" \
        https://github.com/libtcod/libtcod.git "$LIBTCOD_SRC"
fi

echo "==> Configuring libtcod for Windows (static)..."
# Always start with a clean build dir to avoid stale FetchContent patch state
rm -rf "$LIBTCOD_BUILD"
cmake -S "$LIBTCOD_SRC" -B "$LIBTCOD_BUILD" \
    -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN" \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=OFF \
    -DLIBTCOD_SDL3=find \
    -DSDL3_DIR="$SDL3_MINGW/lib/cmake/SDL3" \
    -DCMAKE_INSTALL_PREFIX="$LIBTCOD_INSTALL"

echo "==> Building libtcod..."
cmake --build "$LIBTCOD_BUILD" --parallel

echo "==> Installing libtcod..."
cmake --install "$LIBTCOD_BUILD"

# Copy all static libs and headers produced by the install
cp -f "$LIBTCOD_INSTALL/lib/"*.a "$LIB_DIR/"
cp -rf "$LIBTCOD_INSTALL/include/"* "$WIN_INCLUDE/"

echo ""
echo "Done! Windows cross-compile dependencies are ready."
echo "  Headers : $WIN_INCLUDE"
echo "  Libs    : $LIB_DIR"
echo ""
echo "SDL3.dll (must ship alongside roguelike.exe):"
echo "  $SDL3_MINGW/bin/SDL3.dll"
