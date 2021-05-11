#!/usr/bin/env python3
#
# (c) 2019 Copyright, Real-Time Innovations, Inc.  All rights reserved. RTI
# grants Licensee a license to use, modify, compile, and create derivative
# works of the Software.  Licensee has the right to distribute object form only
# for use with RTI products.  The Software is provided "as is", with no
# warranty of any type, including any warranty for fitness for any purpose. RTI
# is under no obligation to maintain or support the Software.  RTI shall not be
# liable for any incidental or consequential damages arising out of the use or
# inability to use the software.
"""Build examples and perform static analysis.

The environment variable RTI_PACKAGE_VERSION must be assigned to find the
correct RTIConnextDDS package version.

"""
import os
import sys
import time
import datetime
import subprocess

from pathlib import Path


def main():
    time_script_start = time.perf_counter()

    rti_connext_dds_version = os.getenv("RTI_PACKAGE_VERSION")

    if not rti_connext_dds_version:
        print(
            "Environment variable RTI_PACKAGE_VERSION not found, skipping..."
        )
        sys.exit()

    try:
        examples_dir = Path("examples/connext_dds").resolve()
    except FileNotFoundError:
        sys.exit("Error: Examples directory not found.")

    build_dir = examples_dir.joinpath("build")

    try:
        rti_connext_dds_dir = (
            Path.home()
            .joinpath(
                "rti_connext_dds-{}".format(rti_connext_dds_version), "include"
            )
            .resolve()
        )
    except FileNotFoundError:
        sys.exit("Error: RTIConnextDDS not found.")

    if not rti_connext_dds_dir.exists():
        sys.exit("Error: RTIConnextDDS not found.")

    try:
        cmake_build_all_path = Path(
            "resources/cmake/ConnextDdsBuildAllConfigurations.cmake"
        ).resolve()
    except FileNotFoundError:
        sys.exit("Error: Path not found {}.".format(examples_dir))

    if not examples_dir.exists():
        sys.exit("Examples directory not found.")

    if not cmake_build_all_path.exists():
        sys.exit("Cmake script to build all configurations not found.")

    build_dir.mkdir(exist_ok=True)

    if not build_dir.exists():
        sys.exit("Build dir not found.")

    time_build_start = time.perf_counter()

    print("[RTICommunity] Generating build system...", flush=True)

    build_gen_result = subprocess.run(
        [
            "cmake",
            "-DSTATIC_ANALYSIS=ON",
            "-DBUILD_SHARED_LIBS=ON",
            "-DCMAKE_BUILD_TYPE=Release",
            "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON",
            examples_dir,
        ],
        cwd=build_dir,
    )

    if build_gen_result.returncode != 0:
        sys.exit("There was some errors during generating the build system.")

    print("\n[RTICommunity] Compiling the examples...", flush=True)

    building_result = subprocess.run(
        ["cmake", "--build", ".", "--config", "Release"],
        cwd=build_dir,
    )

    if building_result.returncode != 0:
        sys.exit("There was some errors during build.")

    time_build_end = time.perf_counter()

    print("\n[RTICommunity] Analyzing the build...", flush=True)

    time_analysis_start = time.perf_counter()

    static_analysis_result = subprocess.run(
        [
            "analyze-build",
            "--verbose",
            "--status-bugs",
            "--exclude",
            rti_connext_dds_dir,
            "--exclude",
            ".",
            "-o",
            ".",
        ],
        cwd=build_dir,
    )

    time_analysis_end = time.perf_counter()
    time_script_end = time.perf_counter()

    time_script_elapsed = datetime.timedelta(
        seconds=round(time_script_end - time_script_start)
    )
    time_build_elapsed = datetime.timedelta(
        seconds=round(time_build_end - time_build_start)
    )
    time_analysis_elapsed = datetime.timedelta(
        seconds=round(time_analysis_end - time_analysis_start)
    )

    print(
        f"> Build:    {time_build_elapsed}\n"
        f"> Analysis: {time_analysis_elapsed}\n"
        f"{'-' * 19}\n"
        f"> Total:    {time_script_elapsed}\n"
    )

    if static_analysis_result.returncode != 0:
        sys.exit("There where some errors during static analysis.")


if __name__ == "__main__":
    main()
