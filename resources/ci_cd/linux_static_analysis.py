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

import os
import sys
import subprocess
from pathlib import Path


def main():
    rti_connext_dds_version = os.getenv("RTI_PACKAGE_VERSION")
    rti_installation_path = os.getenv("RTI_INSTALLATION_PATH") or Path.home()

    if not rti_connext_dds_version:
        sys.exit(
            "Environment variable RTI_PACKAGE_VERSION not found, skipping..."
        )

    if not rti_installation_path.exists():
        sys.exit("The RTI_INSTALALTION_PATH does not exist.")

    try:
        build_dir = Path("examples/connext_dds/build").resolve(strict=True)
    except FileNotFoundError:
        sys.exit(
            "Error: build directory not found, compile the examples before "
            "running this script."
        )

    try:
        rti_connext_dds_dir = (
            rti_installation_path
            .joinpath(
                "rti_connext_dds-{}".format(rti_connext_dds_version), "include"
            )
            .resolve(strict=True)
        )
    except FileNotFoundError:
        sys.exit("Error: RTIConnextDDS not found.")

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

    if static_analysis_result.returncode != 0:
        sys.exit("There where some errors during static analysis.")


if __name__ == "__main__":
    main()
