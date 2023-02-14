#!/usr/bin/env python3
#
# (c) 2021 Copyright, Real-Time Innovations, Inc.  All rights reserved. RTI
# grants Licensee a license to use, modify, compile, and create derivative
# works of the Software.  Licensee has the right to distribute object form only
# for use with RTI products.  The Software is provided "as is", with no
# warranty of any type, including any warranty for fitness for any purpose. RTI
# is under no obligation to maintain or support the Software.  RTI shall not be
# liable for any incidental or consequential damages arising out of the use or
# inability to use the software.
"""Build all the examples.

The environment variable RTI_PACKAGE_VERSION must be assigned to find the
correct RTIConnextDDS package version.

"""
import argparse
import os
import subprocess
import sys
from pathlib import Path


def cmake_option_conversion(string):
    """Convert a string to a CMake option prepending the -D substring."""
    return f"-D{string}"


def parse_args():
    """Parse the CLI options."""
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-D",
        action="append",
        default=[],
        type=cmake_option_conversion,
        help="CMake options",
        metavar="CMAKE OPTION",
        dest="cmake_options",
    )

    return parser.parse_args()


def main():
    args = parse_args()

    try:
        rti_installation_path = Path(
            os.getenv("RTI_INSTALLATION_PATH") or Path.home()
        ).resolve(strict=True)
    except FileNotFoundError:
        sys.exit("The RTI_INSTALLATION_PATH does not exist.")

    try:
        examples_dir = Path("examples").resolve(strict=True)
    except FileNotFoundError:
        sys.exit("Error: Examples directory not found.")

    found_rti_connext_dds = list(
        rti_installation_path.glob("rti_connext_dds-?.?.?")
    )

    if not found_rti_connext_dds:
        sys.exit("Error: RTIConnextDDS not found.")

    rti_connext_dds_dir = found_rti_connext_dds[0]
    build_dir = examples_dir.joinpath("build")
    build_dir.mkdir(exist_ok=True)
    string_separator = f"\n{'*'*80}\n"

    build_gen_cmd = [
        "cmake",
        "-DSTATIC_ANALYSIS=ON",
        "-DBUILD_SHARED_LIBS=ON",
        "-DCMAKE_BUILD_TYPE=Release",
        "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON",
        "-DCONNEXTDDS_BUILD_PERSISTENCE_SERVICE_EXAMPLES=ON",
        "-DCONNEXTDDS_BUILD_RECORDING_SERVICE_EXAMPLES=ON",
        "-DCONNEXTDDS_BUILD_ROUTING_SERVICE_EXAMPLES=ON",
        "-DCONNEXTDDS_BUILD_WEB_INTEGRATION_SERVICE_EXAMPLES=ON",
        f"-DCONNEXTDDS_DIR={str(rti_connext_dds_dir)}",
        *args.cmake_options,
        str(examples_dir),
    ]

    print(
        "[RTICommunity] Generating build system..."
        f"{string_separator}{' '.join(build_gen_cmd)}{string_separator}",
        flush=True,
    )

    build_gen_result = subprocess.run(
        build_gen_cmd,
        cwd=build_dir,
    )

    if build_gen_result.returncode:
        sys.exit("There were some errors during the build system generation.")

    build_run_cmd = ["cmake", "--build", ".", "--config", "Release"]

    print(
        "\n[RTICommunity] Compiling the examples..."
        f"{string_separator}{' '.join(build_run_cmd)}{string_separator}",
        flush=True,
    )

    build_run_result = subprocess.run(
        build_run_cmd,
        cwd=build_dir,
    )

    if build_run_result.returncode:
        sys.exit("There were some errors during the build.")


if __name__ == "__main__":
    main()
