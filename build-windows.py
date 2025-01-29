#!/usr/bin/env python3

import os
import glob
import sys
import subprocess

script_dir = os.path.dirname(os.path.realpath(__file__))
os.chdir(script_dir)

options = set(sys.argv[1:])

exit_code = 0

command = [
    "cl",
    "/I..\\layers\\",
    "/I..\\local\\",
    "/I..\\thirdparty\\",
    "/nologo",
    "/Z7",
    "/FC",
    "-Gm-",
    "/W4",
    "/wd4100",
    "/wd4201",
]

link_flags = [
    "/link",
    "/MANIFEST:EMBED",
    "/INCREMENTAL:NO",
    "/noexp",
]

# used to check membership in the set, non pruned elements are unused (for warning)
def check_and_remove(options, option):
    result = option in options
    options.discard(option)
    return result

if check_and_remove(options, "release"):
    command += ("-O2", "-DBUILD_DEBUG=0")
    print("[release mode]")
else:
    options.add("debug")

if check_and_remove(options, "debug"):
    command += ("/Od", "/Ob1", "/DBUILD_DEBUG=1")
    link_flags.append("/DEBUG:FULL")
    print("[debug mode]")

if check_and_remove(options, "profile"):
    options.add("tracy")

if check_and_remove(options, "tracy"):
    if "superluminal" in options:
        print("[ERROR] tracy and superluminal profiling enabled - please choose one. Exiting...")
        exit(1)
    command += ("-DPROFILE_TRACY", "/MD")
    print("[tracy profiling enabled]")

if check_and_remove(options, "superluminal"):
    command += ("-DPROFILE_SUPERLUMINAL", "/MD")
    print("[superluminal profiling enabled]")


if check_and_remove(options, "sanitize"):
    options |= {"asan", "RTC"}

if check_and_remove(options, "asan"):
    if "RTC" in options:
        print("[ERROR] RTC and address sanitizer enabled but they're incompatible - please choose one. Exiting...")
        exit(1)
    command.append("/fsanitize=address")
    print("[asan enabled]")

if check_and_remove(options, "RTC") or check_and_remove(options, "rtc"):
    command.append("/RTCsu")
    print("[RTC enabled]")

if check_and_remove(options, "analyze"):
    # REVIEW(beau): should I be doing this?
    # flag = "--analyze:only" if "check" in options else "--analyze"
    command.append("--analyze")
    print("[static analyzer]")

if check_and_remove(options, "check") or check_and_remove(options, "c"):
    command.append("/Zs")
    print("[check only]")

os.makedirs("local", exist_ok=True)
os.makedirs("build", exist_ok=True)
os.chdir("build")

# pdbs accumulate stale debug info
for file in glob.glob("*.pdb"):
    os.remove(file)

build_targets = options & {
    "metagen_main",
    "leetcode",
    "scratch",
    "opengl",
}

for target in build_targets:
    print(f"[build {target}]")
    options.remove(target)
    file_and_out = [f"../local/{target}.c", f"/Fe{target}"]
    this_command = command + file_and_out + link_flags
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
