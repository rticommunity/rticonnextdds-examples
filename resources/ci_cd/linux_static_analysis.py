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
"""Run static analysis over all the built examples.

The environment variable RTI_PACKAGE_VERSION must be assigned to find the
correct RTIConnextDDS package version. Also, the examples must be compiled
to be able to run the static analysis.

"""

import argparse
import os
import subprocess
import sys

from pathlib import Path


def find_connext_dir() -> Path:
    """Try to find the Connext directory from a base installation directory."""
    try:
        rti_installation_path = Path(
            os.getenv("RTI_INSTALLATION_PATH") or Path.home()
        ).resolve(strict=True)
    except FileNotFoundError as error:
        raise FileNotFoundError(
            "The RTI_INSTALLATION_PATH environment variable does not exist:"
            f" {error.filename}"
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


def parse_args() -> argparse.Namespace:
    """Parse the CLI options."""
    parser = argparse.ArgumentParser()
    parser.add_argument("--build-dir", type=Path, default="build")
    parser.add_argument("--connext-dir", type=Path)
    args = parser.parse_args()

    return args


def main():
    args: argparse.Namespace = parse_args()

    try:
        connext_dir: Path = (args.connext_dir or find_connext_dir()).resolve(
            strict=True
        )
    except FileNotFoundError as error:
        error_message = str(error)
        if error.filename:
            error_message = (
                "Could not find the Connext installation directory:"
                f" {error.filename}"
            )
        sys.exit(f"Error: {error_message}")

    try:
        build_dir: Path = args.build_dir.resolve(strict=True)
    except FileNotFoundError as error:
        sys.exit(f"Error: Build directory not found: {error.filename}")

    try:
        connext_include_dir = connext_dir.joinpath("include").resolve(
            strict=True
        )
    except FileNotFoundError as error:
        sys.exit(
            f"Error: Connext include directory not found: {error.filename}"
        )

    static_analysis_result = subprocess.run(
        [
            "analyze-build",
            "--verbose",
            "--status-bugs",
            "--exclude",
            connext_include_dir,
            "--exclude",
            ".",
            "-o",
            ".",
        ],
        cwd=build_dir,
    )

    if static_analysis_result.returncode != 0:
        sys.exit("There where some errors during static analysis.")


if __name__ == "__main__":
    main()
