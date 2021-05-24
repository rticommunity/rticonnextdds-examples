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
import io
import sys
import zipfile

from urllib import request
from pathlib import Path
from zipfile import ZipFile, ZipInfo
from urllib.error import URLError


class ZipFileWithPermissions(ZipFile):
    """Custom ZipFile class handling file permissions.

    Code from https://stackoverflow.com/a/54748564
    """

    def _extract_member(self, member, targetpath, pwd):
        if not isinstance(member, ZipInfo):
            member = self.getinfo(member)

        targetpath = super()._extract_member(member, str(targetpath), pwd)
        attr = member.external_attr >> 16

        if attr != 0:
            Path(targetpath).chmod(attr)

        return targetpath


def main():
    rti_minimal_package_url = os.getenv("RTI_MIN_PACKAGE_URL")

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

    try:
        resp = request.urlopen(rti_minimal_package_url)
    except URLError as e:
        sys.exit("Error opening the URL: {}".format(e))

    print("Extracting minimal installation.")

    try:
        with ZipFileWithPermissions(io.BytesIO(resp.read())) as rti_zipfile:
            bad_file_name = rti_zipfile.testzip()

            if bad_file_name:
                sys.exit("Bad file found in the archive.")

            rti_zipfile.extractall(rti_installation_path)
    except zipfile.BadZipFile as e:
        sys.exit("Error opening zip file: {}".format(e))


if __name__ == "__main__":
    main()
