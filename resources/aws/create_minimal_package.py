#!/usr/bin/env python3.7
#
# (c) 2019 Copyright, Real-Time Innovations, Inc.  All rights reserved. RTI
# grants Licensee a license to use, modify, compile, and create derivative
# works of the Software.  Licensee has the right to distribute object form only
# for use with RTI products.  The Software is provided "as is", with no
# warranty of any type, including any warranty for fitness for any purpose. RTI
# is under no obligation to maintain or support the Software.  RTI shall not be
# liable for any incidental or consequential damages arising out of the use or
# inability to use the software.
"""Create minimal packages of RTI Connext DDS.

Create zip archive with minimal files to allow compilation of the examples.

Examples:
    From installation path:

        python create_minimal_package.py -p /home/user/rti_connext_dds-6.0.0

    From installer archive url:

        python create_minimal_package.py -u https://www.url.example/pkg.taṙ.gz

    From installation path to directory:

        python create_minimal_package.py -p /path -o /tmp

"""
import io
import re
import sys
import logging
import pexpect
import argparse
import tempfile
import tarfile
import zipfile

from typing import List, Tuple, Optional, Dict, Match
from urllib import request
from pathlib import Path
from zipfile import ZipFile
from urllib.error import URLError

HOME_PATH: Path = Path.home()
SCRIPT_PATH: Path = Path(__file__)
TMP_PATH: Path = Path(tempfile.gettempdir()).resolve()

logger: logging.Logger = logging.getLogger(f"{SCRIPT_PATH.name}")
logger.setLevel(logging.DEBUG)

fh = logging.FileHandler(filename=SCRIPT_PATH.with_suffix(".log"), mode="w")
fh.setLevel(logging.DEBUG)
fh.setFormatter(
    logging.Formatter(
        "[%(asctime)s] [%(levelname)s]: %(message)s", "%y/%m/%d-%H:%M"
    )
)
logger.addHandler(fh)

sh = logging.StreamHandler()
sh.setLevel(logging.INFO)
sh.setFormatter(
    logging.Formatter("[%(levelname)s]: %(message)s", "%y/%m/%d-%H:%M")
)
logger.addHandler(sh)


class InstallerNotFoundError(FileNotFoundError):
    pass


class InstallationNotFoundError(FileNotFoundError):
    pass


class ArchitectureNotFoundError(FileNotFoundError):
    pass


class VersionNotFoundError(FileNotFoundError):
    pass


def get_argument_parser() -> argparse.ArgumentParser:
    """Create argument parser.

    Returns:
        The argument parser.

    """
    parser = argparse.ArgumentParser(
        description=__doc__,
        allow_abbrev=False,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )

    mutual = parser.add_mutually_exclusive_group(required=True)
    mutual.add_argument(
        "-u",
        "--rti-connextdds-url",
        type=str,
        metavar="URL",
        dest="rti_connextdds_url",
        help="RTI Connext DDS installer URL.",
    )
    mutual.add_argument(
        "-i",
        "--rti-installer-path",
        type=Path,
        metavar="path",
        dest="rti_installer_path",
        help="RTI Connext DDS installer Path.",
    )
    mutual.add_argument(
        "-p",
        "--rti-connextdds-path",
        type=Path,
        metavar="path",
        dest="rti_connextdds_path",
        help="RTI Connext DDS installation directory.",
    )
    parser.add_argument(
        "-o",
        "--output-path",
        type=Path,
        required=False,
        metavar="path",
        dest="output_path",
        help="Output zip path.",
    )

    logger.debug("Created argument parser.")

    return parser


def guess_architecture_from_installation(installation_path: Path) -> str:
    """Guess target architecture based on lib directory.

    Args:
        installation_path: RTI Connext DDS installation path.

    Returns:
        Target architecture.

    Raises:
        InstallationNotFoundError: When installation directory does not exist.
        ArchitectureNotFoundError: When there is no target architecture
            installed.

    """
    try:

        if not installation_path.exists():

            raise InstallationNotFoundError(
                "Installation not found in destination."
            )

    except InstallationNotFoundError as e:

        logger.debug(e, stack_info=True)
        raise

    architecture_list: List[str] = [
        directory.name
        for directory in installation_path.joinpath("lib").iterdir()
        if directory.is_dir() and directory.name != "java"
    ]

    arch_number: int = len(architecture_list)
    try:

        if arch_number == 0:

            raise ArchitectureNotFoundError(
                "No architecture_list found in the installation directory."
            )

    except ArchitectureNotFoundError as e:

        logger.debug(e, stack_info=True)
        raise

    architecture: Optional[str] = None

    if arch_number > 1:

        architecture = None

        while architecture not in architecture_list:

            logger.info(
                f"Select architecture: {' , '.join(architecture_list)}"
            )
            architecture = input("> ")

    else:

        architecture = architecture_list[0]

    logger.debug(f"Guessed architecture: {architecture}.")

    return architecture


def guess_version_from_name(name: str) -> str:
    """Guess package version based on file/diretory name.

    Args:
        name: String to search the version from

    Returns:
        Version string.

    Raises:
        VersionNotFoundError: When could not find version in the string.

    """
    version_regex: str = r"-([0-9]\.[0-9]\.[0-9])-?"

    vers_search: Optional[Match] = re.search(version_regex, name)
    try:

        if not vers_search:

            raise VersionNotFoundError(
                "No version found in the installer / "
                "installation directory name."
            )

    except VersionNotFoundError as e:

        logger.debug(e, stack_info=True)
        raise

    version = vers_search.groups()[0]

    logger.debug(f"Guessed version: {version}.")

    return version


def guess_architecture_from_name(name: str) -> str:
    """Guess package architecture based on file/diretory name.

    Args:
        name: String to search the architecture from

    Returns:
        Architecture string.

    Raises:
        ArchitectureNotFoundError: When could not find architecture in the
            string.

    """
    architecture_regex: str = r"-([a-zA-Z0-9\.]+)\.(?:tar|zip)"

    arch_search: Optional[Match] = re.search(architecture_regex, name)

    try:

        if not arch_search:

            raise ArchitectureNotFoundError(
                "No architecture found in the installer."
            )

    except ArchitectureNotFoundError as e:

        logger.debug(e, stack_info=True)
        raise

    architecture = arch_search.groups()[0]

    logger.debug(f"Guessed architecture: {architecture}.")

    return architecture


def download_extract_installer(
    installer_archive_url: str, installer_output_path: Path
) -> Tuple[Path, Dict[str, str]]:
    """Download and extract the evaluation package installer.

    Args:
        installer_archive_url: RTI Connext DDS installer URL.
        installer_output_path: RTI Connext DDS installer output path.

    Returns:
        Path to installer.
        Dictionary with target version and architecture.

    Raises:
        VersionNotFoundError: When could not find version in the string.
        ArchitectureNotFoundError: When could not find architecture in the
            string.
        URLError: On bad url.
        tarfile.ReadError: On error reading tar.gz archive.
        zipfile.BadZipFile: On error reading zip archive.

    """
    logger.info(
        f"Donwloading and exctacting installer to {installer_output_path}..."
    )

    pkg_info: Dict[str, str] = {}
    archive_name: str = installer_archive_url.split("/")[-1]

    try:

        logger.debug("Guessing version...")
        pkg_info["version"] = guess_version_from_name(archive_name)

    except VersionNotFoundError as e:

        logger.debug(e, stack_info=True)
        raise

    try:

        logger.debug("Guessing architecture...")
        pkg_info["architecture"] = guess_architecture_from_name(archive_name)

    except ArchitectureNotFoundError as e:

        logger.debug(e, stack_info=True)
        raise

    try:
        logger.debug("Getting url...")
        resp = request.urlopen(installer_archive_url)

    except URLError as e:

        logger.debug(e, stack_info=True)
        raise

    if installer_archive_url.endswith(".tar.gz"):
        try:

            logger.debug("Extracting tar.gz archive...")
            with tarfile.open(fileobj=io.BytesIO(resp.read())) as rti_tarfile:

                rti_tarfile.extractall(installer_output_path)

        except tarfile.ReadError as e:

            logger.debug(e, stack_info=True)
            raise

        installer_extension = "run"

    elif installer_archive_url.endswith(".zip"):
        try:

            logger.debug("Extracting zip archive...")

            with ZipFile(io.BytesIO(resp.read())) as rti_zipfile:

                bad_file_name = rti_zipfile.testzip()

                if bad_file_name:
                    sys.exit("Bad file found in the archive.")

                rti_zipfile.extractall(installer_output_path)

        except zipfile.BadZipFile as e:

            logger.debug(e, stack_info=True)
            raise

        installer_extension = "exe"

    return (
        installer_output_path.joinpath(
            f"rti_connext_dds-{pkg_info.get('version')}"
            f"-eval-{pkg_info.get('architecture')}"
            f".{installer_extension}"
        ),
        pkg_info,
    )


def install_package(installer_path: Path, installation_parent_path: Path):
    """Install package into local machine.

    Args:
        installer_path: RTI Connext DDS installer path.
        installation_parent_path: RTI Connext DDS parent installation path.

    Returns:

    Raises:
        InstallerNotFoundError: When installer path does not exist.
        pexpect.TIMEOUT: On timeout during the installation.

    """
    logger.info(f"Installing package in {installation_parent_path}...")

    try:

        if not installer_path.exists():

            raise InstallerNotFoundError(
                f"Installer {installer_path} does not exist."
            )

    except InstallerNotFoundError as e:

        logger.debug(e, stack_info=True)
        raise

    try:

        child: pexpect.spawn = pexpect.spawn(
            str(installer_path), ["--mode", "text"]
        )
        child.delaybeforesend = 0.5

        while True:

            index: int = child.expect(
                [r"Installation Directory.+:", r"(Press|Do you).+:"]
            )

            if index == 0:

                child.sendline(str(installation_parent_path))
                break

            else:

                child.sendline("y")

        child.expect(r"Do.+:")
        child.sendline("y")

        child.expect(r"Disable.+:", timeout=120)
        child.sendline("y")

        child.expect(r"Create.+:")
        child.sendline("n")

        child.expect(pexpect.EOF)

    except pexpect.TIMEOUT as e:

        logger.debug(e, stack_info=True)
        raise


def select_files_for_minimun_package(installation_path):
    """Obtain files for minimal package.

    Args:
        installation_path: RTI Connext DDS installation path.

    Returns:
        Chosen files.

    Raises:
        FileNotFoundError: When no files are found.

    """
    logger.debug("Getting minimal files...")

    globs = (
        "rti_versions.xml",
        "bin/rtiddsgen",
        "lib/**/*",
        "include/**/*",
        "resource/app/app_support/rtiddsgen/**/*",
        "resource/app/jre/**/*",
        "resource/app/lib/java/**/*",
        "resource/scripts/**/*",
    )

    selected_files: List[Path] = []

    for rel in globs:

        selected_files.extend(
            filter(lambda x: x.is_file(), installation_path.glob(rel))
        )

    try:

        if not selected_files:

            raise FileNotFoundError("No files found with selected globs.")

    except FileNotFoundError as e:

        logger.debug(e, stack_info=True)
        raise

    return selected_files


def create_minimal_package(
    selected_files: List[Path],
    pkg_info: Dict[str, str],
    archive_parent_path: Path,
    installation_path: Path,
) -> None:
    """Create minimal zip of a Connext DDS package.

    Args:
        selected_files: obtained files for minimal package.
        pkg_info: Dictionary with target version and architecture.
        archive_parent_path: Minimal archive destination directory.
        installation_path: RTI Connext DDS installation path.

    Raises:
        zipfile.BadZipFile: On error creating zip archive.

    """
    logger.info(f"Creating minimal package in {archive_parent_path}...")

    vers = pkg_info.get("version")
    arch = pkg_info.get("architecture")

    zip_name = f"rti_connext_dds-{vers}-min-{arch}.zip"

    try:

        with ZipFile(
            archive_parent_path.joinpath(zip_name), "w", zipfile.ZIP_DEFLATED
        ) as z:

            for file_to_compress in selected_files:
                z.write(
                    file_to_compress,
                    file_to_compress.relative_to(installation_path.parent),
                )

    except zipfile.BadZipFile as e:

        logger.debug(e, stack_info=True)
        raise


def main():
    args = get_argument_parser().parse_args()

    pkg_info: Dict[str, str] = {}

    installer_path: Path = None

    if args.output_path:

        output_path: Path = args.output_path.resolve()

    else:

        output_path: Path = TMP_PATH

    if args.rti_connextdds_url:

        try:

            installer_path, pkg_info = download_extract_installer(
                args.rti_connextdds_url, output_path
            )

        except VersionNotFoundError:

            sys.exit("Error: No version found.")

        except ArchitectureNotFoundError:

            sys.exit("Error: No architecture found.")

        except URLError:

            sys.exit("Error openning the url")

        except (tarfile.ReadError, zipfile.BadZipFile):

            sys.exit("Error with the archive.")

    elif args.rti_installer_path:

        installer_path = args.rti_installer_path.resolve()

    if installer_path:

        if not pkg_info.get("version"):

            try:

                pkg_info["version"] = guess_version_from_name(
                    installer_path.name
                )

            except VersionNotFoundError:

                sys.exit("Error: No version found.")

        try:

            install_package(installer_path, output_path)

        except InstallerNotFoundError:

            sys.exit("Error: Installer path not found.")

        except pexpect.TIMEOUT:

            sys.exit("Error: Pexpect returnted timeout.")

        rti_connextdds_path = output_path.joinpath(
            f"rti_connext_dds-{pkg_info.get('version')}"
        )

    elif args.rti_connextdds_path:

        rti_connextdds_path = args.rti_connextdds_path.resolve()

    else:

        sys.exit("Error: No url or path provided.")

    if not rti_connextdds_path.exists():

        sys.exit(f"Path {rti_connextdds_path} does not exist.")

    if not pkg_info.get("version"):

        try:

            pkg_info["version"] = guess_version_from_name(
                rti_connextdds_path.name
            )

        except VersionNotFoundError:

            sys.exit("Error: No version found.")

    if not pkg_info.get("architecture"):

        try:

            pkg_info["architecture"] = guess_architecture_from_installation(
                rti_connextdds_path
            )

        except ArchitectureNotFoundError:

            sys.exit("Error: No architecture found.")

    try:

        selected_files = select_files_for_minimun_package(rti_connextdds_path)

    except FileNotFoundError:

        sys.exit("Error: No files found in the directory.")

    try:

        create_minimal_package(
            selected_files, pkg_info, output_path, rti_connextdds_path
        )

    except zipfile.BadZipFile:

        sys.exit("Error: exception creating zip file.")


if __name__ == "__main__":
    main()
