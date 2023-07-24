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
import os
import sys
import tempfile
import urllib.request

from distutils.spawn import find_executable
from pathlib import Path
from subprocess import call
from urllib.error import URLError


def main():
    rti_minimal_package_url = os.getenv("RTI_MIN_PACKAGE_URL")
    temp_dir = Path(tempfile.gettempdir())
    if not rti_minimal_package_url:
        sys.exit(
            "Environment variable RTI_MIN_PACKAGE_URL not found, skipping..."
        )

    try:
        rti_installation_path = Path(
            os.getenv("RTI_INSTALLATION_PATH") or Path.home()
        ).resolve(strict=True)
    except FileNotFoundError:
        sys.exit("The RTI_INSTALLATION_PATH does not exist.")

    cmake_command = find_executable("cmake")
    if cmake_command is None:
        sys.exit("CMake must be installed in order to use the script.")

    try:
        rti_zipped_file_name = Path(
            urllib.request.url2pathname(rti_minimal_package_url)
        ).name
        rti_zipped_file_path = temp_dir.joinpath(rti_zipped_file_name)
        urllib.request.urlretrieve(
            rti_minimal_package_url, rti_zipped_file_path
        )
    except URLError as e:
        sys.exit("Error opening the URL: {}".format(e))

    try:
        return_value = call(
            [
                cmake_command,
                "-E",
                "tar",
                "xf",
                str(rti_zipped_file_path),
                "--format=zip",
            ],
            cwd=rti_installation_path,
        )
    except FileNotFoundError:
        sys.exit("The CMake executable could not be found.")

    if return_value:
        sys.exit("There were error extracting the package")


if __name__ == "__main__":
    main()
