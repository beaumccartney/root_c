#!/usr/bin/env python3

import os
import sys
import subprocess

script_dir = os.path.dirname(os.path.realpath(__file__))
os.chdir(script_dir)

options = set(sys.argv[1:])
if "release" not in options:
    options.add("debug")

command = [
    "cc",
    "-I../layers/",
    "-I../local/",
    "-I../thirdparty/",
    "-fdiagnostics-absolute-paths",
    "-Wall",
    "-Wextra",
    "-Wimplicit-int-conversion",
    "-Wno-unused-function",
    "-Wno-missing-braces",
    "-Wno-unused-parameter",
    "-Wconversion",
    "-Wdouble-promotion",
    "-ObjC",
    "-fno-objc-arc",
    "-framework",
    "Cocoa",
    "-framework",
    "Metal",
    "-framework",
    "QuartzCore",
]

# used to check membership in the set, non pruned elements are unused (for warning)
def check_and_remove(options, option):
    result = option in options
    options.discard(option)
    return result

if check_and_remove(options, "debug"):
    command += ("-g3", "-O0", "-DBUILD_DEBUG=1")
    print("[debug mode]")

if check_and_remove(options, "release"):
    command += ("-O2", "-DBUILD_DEBUG=0")
    print("[release mode]")

if check_and_remove(options, "profile"):
    options.add("tracy")
if check_and_remove(options, "tracy"):
    command += ("-lc++", "-DPROFILE_TRACY", "../thirdparty/tracy/mac/libTracyClient.a")
    print("[tracy profiling enabled]")

if check_and_remove(options, "sanitize"):
    options |= {"asan", "ubsan"}
if check_and_remove(options, "asan"):
    command.append("-fsanitize=address")
    print("[asan enabled]")
if check_and_remove(options, "ubsan"):
    command.append("-fsanitize=undefined")
    print("[ubsan enabled]")

os.makedirs("build", exist_ok=True)
os.chdir("build")

build_targets = options & {
    "leetcode",
    "metal",
}

if not build_targets:
    print("[WARNING] no valid build target specified")
    exit(1)

for target in build_targets:
    print(f"[build {target}]")
    options.remove(target)
    this_command = command + [f"../local/{target}.c"]
    if "verbose" in options:
        print(" ".join(this_command))
    subprocess.run(this_command)

options.discard("verbose")

if options:
    unused_string = ", ".join([f"'{option}'" for option in options])
    print("[WARNING] unused option(s):", unused_string)
    exit(1)
