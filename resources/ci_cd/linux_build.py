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


def find_connext_dir() -> Path:
    """Try to find the Connext dir from a base installation directory."""
    try:
        rti_installation_path = Path(
            os.getenv("RTI_INSTALLATION_PATH") or Path.home()
        ).resolve(strict=True)
    except FileNotFoundError as error:
        raise FileNotFoundError(
            f"The RTI_INSTALLATION_PATH environment variable does not exist: {error.filename}"
        ) from error

    found_rti_connext_dds = list(
        rti_installation_path.glob("rti_connext_dds-?.?.?")
    )

    if not found_rti_connext_dds:
        raise FileNotFoundError(
            "No Connext installations found in the"
            f" {str(rti_installation_path)} directory"
        )

    return found_rti_connext_dds.pop()


def parse_args():
    """Parse the CLI options."""
    parser = argparse.ArgumentParser()
    parser.add_argument("--build-dir", type=str, default="build")
    parser.add_argument("--build-mode", type=str, choices=("release", "debug"), default="release")
    parser.add_argument("--link-mode", type=str, choices=("dynamic", "static"), default="dynamic")
    parser.add_argument("--connext-dir", type=Path)
    parser.add_argument(
        "-D",
        action="append",
        default=[],
        type=cmake_option_conversion,
        help="CMake options",
        metavar="CMAKE OPTION",
        dest="cmake_options",
    )
    args = parser.parse_args()
    args.shared_libs = "ON" if args.link_mode == "dynamic" else "OFF"
    args.build_mode = args.build_mode.capitalize()

    return args


def main():
    args = parse_args()

    try:
        examples_dir: Path = Path("examples").resolve(strict=True)
    except FileNotFoundError as error:
        sys.exit(f"Error: Examples directory not found: {error.filename}")

    try:
        connext_dir: Path = (args.connext_dir or find_connext_dir()).resolve(strict=True)
    except FileNotFoundError as error:
        error_message = str(error)
        if error.filename:
            error_message = f"Could not find the connext dir: {error.filename}"
        sys.exit(f"Error: {error_message}")

    build_dir_path = examples_dir.joinpath(args.build_dir)
    build_dir_path.mkdir(exist_ok=True)
    string_separator = f"\n{'*'*80}\n"

    build_gen_cmd = [
        "cmake",
        "-DSTATIC_ANALYSIS=ON",
        f"-DBUILD_SHARED_LIBS={args.shared_libs}",
        f"-DCMAKE_BUILD_TYPE={args.build_mode}",
        "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON",
        "-DCONNEXTDDS_BUILD_PERSISTENCE_SERVICE_EXAMPLES=ON",
        "-DCONNEXTDDS_BUILD_RECORDING_SERVICE_EXAMPLES=ON",
        "-DCONNEXTDDS_BUILD_ROUTING_SERVICE_EXAMPLES=ON",
        "-DCONNEXTDDS_BUILD_CLOUD_DISCOVERY_SERVICE_EXAMPLES=ON",
        "-DCONNEXTDDS_BUILD_WEB_INTEGRATION_SERVICE_EXAMPLES=ON",
        f"-DCONNEXTDDS_DIR={str(connext_dir)}",
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
        cwd=build_dir_path,
    )

    if build_gen_result.returncode:
        sys.exit("There were some errors during the build system generation.")

    build_run_cmd = ["cmake", "--build", ".", "--config", args.build_mode]

    print(
        "\n[RTICommunity] Compiling the examples..."
        f"{string_separator}{' '.join(build_run_cmd)}{string_separator}",
        flush=True,
    )

    build_run_result = subprocess.run(
        build_run_cmd,
        cwd=build_dir_path,
    )

    if build_run_result.returncode:
        sys.exit("There were some errors during the build.")


if __name__ == "__main__":
    main()
