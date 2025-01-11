#!/usr/bin/env zsh

set -eu
cd "$(dirname "$0")"

for arg in "$@"; do declare $arg='1'; done
if [ ! -v release ]; then debug=1; fi

# common flags
flags=(
	"-I../layers/"
	"-I../local/"
	"-I../thirdparty/"
	"-fdiagnostics-absolute-paths"
	"-Wall"
	"-Wextra"
	"-Wimplicit-int-conversion"
	"-Wno-unused-function"
	"-Wno-missing-braces"
	"-Wno-unused-parameter"
	"-Wconversion"
	"-Wdouble-promotion"
	"-ObjC"
	"-fno-objc-arc"
	"-framework"
	"Cocoa"
	"-framework"
	"Metal"
	"-framework"
	"QuartzCore"
)

# debug/release flags
if [ -v debug   ]; then flags+=("-g3" "-O0" "-DBUILD_DEBUG=1") && echo "[debug mode]"; fi
if [ -v release ]; then flags+=("-O2" "-DBUILD_DEBUG=0")       && echo "[release mode]"; fi

if [ -v profile ]; then tracy=1; fi
if [ -v tracy   ]; then flags+=("-lc++" "-DPROFILE_TRACY" "../thirdparty/tracy/mac/libTracyClient.a") && echo "[tracy profiling enabled]"; fi

# sanitizer flags
# TODO(beau): -fsanitize-trap toggle?
if [ -v sanitize ]; then asan=1 && ubsan=1; fi
if [ -v asan     ]; then flags+=("-fsanitize=address")   && echo "[asan enabled]"; fi
if [ -v ubsan    ]; then flags+=("-fsanitize=undefined") && echo "[ubsan enabled]"; fi

mkdir -p build local

cd build

# set -x # uncomment to print build command
cc $flags -o test ../local/main.c
