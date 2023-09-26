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
"""Installation of RTIConnextDDS before static analysis.

The environment variable RTI_MIN_PACKAGE_URL must be assigned for downloading
the minimal installation archive.

"""
import argparse
import os
import sys
import tempfile
from distutils.spawn import find_executable
from pathlib import Path, PurePosixPath
from subprocess import call
from urllib.error import URLError

import boto3

ROOT_DIR = Path(__file__).resolve().parents[2]


def parse_args() -> argparse.Namespace:
    """Parse the CLI options."""
    parser = argparse.ArgumentParser()
    parser.add_argument("-a", "--architecture", type=str, required=True)
    parser.add_argument(
        "-i",
        "--install-parent-dir",
        type=Path,
        default=os.getenv("RTI_INSTALLATION_PATH") or Path.home(),
    )
    parser.add_argument(
        "-b", "--bucket", type=str, default=os.getenv("RTI_AWS_BUCKET")
    )
    parser.add_argument(
        "-p",
        "--s3-path",
        type=PurePosixPath,
        default=os.getenv("RTI_AWS_PATH"),
    )

    args = parser.parse_args()

    return args


def main():
    args = parse_args()
    version = ROOT_DIR.joinpath("VERSION").read_text()
    downloaded_zipped_file_path = Path(
        tempfile.gettempdir(),
        f"rti_connext_dds-{version}-lm-{args.architecture}.zip",
    )
    remote_zipped_file_path = PurePosixPath(
        args.s3_path, version, downloaded_zipped_file_path.name
    )
    s3 = boto3.client("s3")

    try:
        rti_install_parent_dir = Path(args.install_parent_dir).resolve(
            strict=True
        )
    except FileNotFoundError:
        sys.exit(f"File not found: {args.install_parent_dir}")

    cmake_command = find_executable("cmake")
    if cmake_command is None:
        sys.exit("CMake must be installed in order to use the script.")

    try:
        with open(downloaded_zipped_file_path, "wb") as f:
            s3.download_fileobj(args.bucket, str(remote_zipped_file_path), f)
    except URLError as e:
        sys.exit(f"Error opening the object: {e}")

    try:
        return_value = call(
            [
                cmake_command,
                "-E",
                "tar",
                "xf",
                str(downloaded_zipped_file_path),
                "--format=zip",
            ],
            cwd=rti_install_parent_dir,
        )
    except FileNotFoundError:
        sys.exit("The CMake executable could not be found.")

    if return_value:
        sys.exit("There were error extracting the package")


if __name__ == "__main__":
    main()
