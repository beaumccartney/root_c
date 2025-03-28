#!/usr/bin/env python3

import os
import sys
import subprocess

script_dir = os.path.dirname(os.path.realpath(__file__))
os.chdir(script_dir)

options = set(sys.argv[1:])
if "release" not in options:
    options.add("debug")

exit_code = 0

command = [
    "cc",
    "-I../layers/",
    "-I../local/",
    "-I../thirdparty/",
    "-fdiagnostics-absolute-paths",
    "-fms-extensions",
    "-Wall",
    "-Wextra",
    "-Wshadow",
    "-Wstrict-prototypes",
    "-Wvla",
    "-Wimplicit-int-conversion",
    "-Wno-unused-function",
    "-Wno-missing-braces",
    "-Wno-unused-parameter",
    "-Wconversion",
    "-Wdouble-promotion",
]

if "metal" in options and options.isdisjoint({"analyze", "check", "c"}):
    command += (
        "-framework",
        "Cocoa",
        "-framework",
        "Metal",
        "-framework",
        "QuartzCore",
        "-ObjC",
        "-fno-objc-arc",
    )

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
    command += ("-DPROFILE_TRACY", "../thirdparty/tracy/mac/libTracyClient.a")
    print("[tracy profiling enabled]")

if check_and_remove(options, "sanitize"):
    options |= {"asan", "ubsan"}
if check_and_remove(options, "asan"):
    command.append("-fsanitize=address")
    print("[asan enabled]")
if check_and_remove(options, "ubsan"):
    command.append("-fsanitize=undefined")
    print("[ubsan enabled]")

if check_and_remove(options, "check") or check_and_remove(options, "c"):
    command.append("-fsyntax-only")
    print("[check only]")

if check_and_remove(options, "pedantic"):
    command.append("-pedantic")
    print("[pedantic warnings]")
    if "analyze" in options:
        print("[WARNING] static analyzer and pedantic warnings enabled; pedantic warnings will be suppressed.")
if check_and_remove(options, "analyze"):
    command.append("--analyze")
    print("[static analyzer]")

os.makedirs("local", exist_ok=True)
os.makedirs("build", exist_ok=True)
os.chdir("build")

build_targets = options & {
    "metagen",
    "scratch",
    "metal"
}

for target in build_targets:
    print(f"[build {target}]")
    options.remove(target)
    file_and_out = [f"../{target}/{target}_main.c", "-o", target]
    this_command = command + file_and_out
    if "verbose" in options:
        print(" ".join(this_command))
    subprocess.run(this_command)

options.discard("verbose")

if not build_targets:
    print("[WARNING] no valid build target specified")
    exit_code = 1

if options:
    unused_string = ", ".join([f"'{option}'" for option in options])
    print("[WARNING] unused option(s):", unused_string)
    exit_code = 1

exit(exit_code)
