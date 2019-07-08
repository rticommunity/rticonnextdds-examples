import os
import io
import sys
import zipfile

from urllib import request
from pathlib import Path
from zipfile import ZipFile, ZipInfo
from urllib.error import URLError

HOME_PATH: Path = Path.home()


class ZipFileWithPermissions(ZipFile):
    """Custom ZipFile class handling file permissions.

    Code from https://stackoverflow.com/a/54748564
    """

    def _extract_member(self, member, targetpath, pwd):

        if not isinstance(member, ZipInfo):

            member = self.getinfo(member)

        targetpath = super()._extract_member(member, targetpath, pwd)

        attr = member.external_attr >> 16

        if attr != 0:

            Path(targetpath).chmod(attr)

        return targetpath


def main():
    rti_minimal_package_url: str = os.getenv("RTI_MIN_PACKAGE_URL")

    if not rti_minimal_package_url:
        sys.exit(
            "Provide url with the environment variable RTI_MIN_PACKAGE_URL"
        )

    try:

        resp = request.urlopen(rti_minimal_package_url)

    except URLError as e:

        sys.exit(f"Error opening the URL: {e}")

    try:

        with ZipFileWithPermissions(io.BytesIO(resp.read())) as rti_zipfile:

            bad_file_name = rti_zipfile.testzip()

            if bad_file_name:

                sys.exit("Bad file found in the archive.")

            rti_zipfile.extractall(HOME_PATH)

    except zipfile.BadZipFile as e:

        sys.exit(f"Error opening zip file: {e}")


if __name__ == "__main__":
    main()
