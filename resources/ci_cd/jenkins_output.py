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

"""
Creates a Markdown file with the text and information for the GitHub check
details URL.
"""

import os
import platform
import subprocess
from pathlib import Path


def main():
    rti_installation_path = Path(
        os.getenv("RTI_INSTALLATION_PATH") or Path.home()
    )

    found_rti_connext_dds = list(
        rti_installation_path.glob("rti_connext_dds-?.?.?")
    )

    if not found_rti_connext_dds:
        rti_package_version = "-"
    else:
        rti_connext_dds_path = str(found_rti_connext_dds[0])
        rti_package_version = rti_connext_dds_path.split("-")[-1]
        os.environ["NDDSHOME"] = str(rti_connext_dds_path)

    with open("resources/markdown_templates/JenkinsTemplate.md", "r") as file:
        text = file.read()

    text = text.replace("@RTI_PACKAGE_VERSION@", rti_package_version)
    text = text.replace("@OPERATING_SYSTEM@", platform.system())
    text = text.replace("@MACHINE_TYPE@", platform.machine())
    text = text.replace("@OS_VERSION@", platform.release())

    if platform.system() == "Linux":
        gcc_version = (
            subprocess.run(
                ["gcc", "-dumpfullversion"],
                capture_output=True,
            )
            .stdout[:-1]
            .decode("utf-8")
            or "-"
        )

        extra_rows = f"| GCC Version | {gcc_version} |\n"
        clang_version = (
            subprocess.run(
                ["clang", "-dumpversion"], capture_output=True, check=True
            )
            .stdout[:-1]
            .decode("utf-8")
            or "-"
        )

        extra_rows += f"| CLANG Version | {clang_version} |\n"
    elif platform.system() == "Darwin":
        output = (
            subprocess.run(
                ["clang", "--version"], capture_output=True, check=True
            ).stdout.decode("utf-8")
            or "-"
        )
        clang_version = output.split("\n")[0]

        extra_rows = f"| CLANG Version | {clang_version} |\n"

    text = text.replace("@EXTRA_ROWS@", extra_rows)

    with open("Dockerfile", "r") as file:
        dockerfile = file.read()
        text = text.replace("@DOCKERFILE@", dockerfile)

    with open("Jenkinsfile", "r") as file:
        jenkinsfile = file.read()
        text = text.replace("@JENKINSFILE@", jenkinsfile)

    rti_logs_file = os.getenv("RTI_LOGS_FILE")
    replace = None

    if rti_logs_file is not None:
        logs_path = Path(rti_logs_file)

        LINES_TO_WRITE = 100
        if logs_path.is_file():
            replace = open(logs_path, "r").readlines()
            replace = "".join(replace[-LINES_TO_WRITE:])
            logs_path.unlink()

    text = text.replace("@LOGS@", replace or "There are no logs for this job")

    with open("jenkins_output.md", "w") as file:
        file.write(text)


if __name__ == "__main__":
    main()
