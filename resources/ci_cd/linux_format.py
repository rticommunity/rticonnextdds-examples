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
"""Ensure correct format of modified files.

This module performs linting for staged changes or changes between two commits
and exits if any linter fails. Currently supporting ``clang-format`` and
``markdownlint``. The default behaviour is to check stashed changes only.

Providing commit and commit range runs only work fine for clang-format which
checks changed lines from those commits. Other linters just check hole files in
the current state ignoring commit changes.

To disable any of the formatters you can use the propper
``--disable-<formater>`` flag. Check ``--help`` flag for more info.

Examples: Run linters on staged changes::

        ./linux_format.py

    Run linters for a commit::

        ./linux_format.py -c 8c0859a

    Run linters for changes in between two commits::

        ./linux_format.py -r "8c0859a 487df86"

        ./linux_format.py -r 8c0859a...487df86

    Disable on linter::

        ./linux_format.py -c 8c0859a --disable-markdownlint

"""
import sys

from argparse import ArgumentParser, RawDescriptionHelpFormatter, Namespace
from pathlib import Path
from typing import List, Set, Optional
import subprocess


class FormatError(Exception):
    """Formating error."""


def get_argument_parser() -> ArgumentParser:
    """Create argument parser.

    :returns: The argument parser.

    """
    parser = ArgumentParser(
        description=__doc__,
        allow_abbrev=False,
        formatter_class=RawDescriptionHelpFormatter,
    )
    mutual = parser.add_mutually_exclusive_group(required=False)
    mutual.add_argument(
        "-r",
        "--commit-range",
        type=str,
        metavar="hash_range",
        dest="commit_range",
        help="Store commits hash range.",
    )
    mutual.add_argument(
        "-c",
        "--commit",
        type=str,
        metavar="hash",
        dest="commit",
        help="Store commit hash.",
    )
    mutual.add_argument(
        "-a",
        "--all",
        action="store_true",
        dest="all_files",
        help="Run the linters for all the files.",
    )
    parser.add_argument(
        "-md",
        "--disable-markdownlint",
        action="append_const",
        dest="disabled_linters",
        const="markdownlint",
        default=[],
        help="Disable markdownlint linter.",
    )
    parser.add_argument(
        "-clang",
        "--disable-clang-format",
        action="append_const",
        dest="disabled_linters",
        const="clang-format",
        default=[],
        help="Disable clang-format linter.",
    )
    parser.add_argument(
        "-py",
        "--disable-black",
        action="append_const",
        dest="disabled_linters",
        const="black",
        default=[],
        help="Disable black linter.",
    )

    return parser


def get_git_files(
    repo_root: Path,
    commits: Optional[List[str]] = None,
    all_files: bool = False,
) -> List[str]:
    """Obtain changed files.

    :param repo_root: Repository top folder.
    :param commits: Commits range.
    :returns: List of staged files.

    """
    command = []

    if all_files:
        command.append("ls-files")
        print("Getting all the repository files.")
    else:
        command = ["diff", "--no-commit-id", "--name-only", "-r"]

        if commits:
            command.extend([*commits])
            print(f"Checking file changes in the commit range: {commits}.")
        else:
            command.insert(3, "--cached")
            command.append("HEAD")
            print("Checking local changed files...")

    command.insert(0, "git")

    result = subprocess.run(command, cwd=repo_root, capture_output=True)

    changed_files = result.stdout.decode("utf-8")
    changed_files = changed_files[:-1].split("\n")

    return changed_files


def filter_files(file_list: List[str], extensions: Set[str]) -> List[str]:
    """Filter files with selected extensions.

    :param file_list: List of files.
    :param extensions: Set of extensions.
    :returns: Filtered list of files.

    """
    return [
        current_file
        for current_file in file_list
        if Path(current_file).suffix in extensions
    ]


def perform_linting(repo_root: Path, command: List[str]) -> None:
    """Perform ``command`` linter to ``files``.

    :param repo_root: Repository top folder.
    :param command: List of commands to run the linter.
    :raises FormatError: When the linter outputs format errors.

    """
    lint_result = subprocess.run(command, cwd=repo_root, capture_output=True)

    if lint_result.returncode != 0 or len(lint_result.stdout) > 1:
        print(lint_result.stdout.decode("utf-8"))
        print(lint_result.stderr.decode("utf-8"))
        raise FormatError(f"Format error for {command} command.")


if __name__ == "__main__":
    args: Namespace = get_argument_parser().parse_args()
    current_script_dir: Path = Path(__file__).parent.resolve()

    result = subprocess.run(
        ["git", "rev-parse", "--show-toplevel"],
        cwd=current_script_dir,
        capture_output=True,
    )

    if result.returncode != 0:
        sys.exit("Error: Script not in a git repository.")

    try:
        repo_root: Path = Path(result.stdout[:-1].decode("utf-8")).resolve(
            strict=True
        )
    except FileNotFoundError:
        sys.exit("Error: Root repo path not found.")

    print("Starting format checker...")

    commits: Optional[List[str]] = None
    exit_error: bool = False

    if args.commit_range:
        commits = args.commit_range.replace("...", " ").split()
    elif args.commit:
        commits = [args.commit + "^", args.commit]

    file_list: List[str] = get_git_files(repo_root, commits, args.all_files)

    if not file_list:
        sys.exit("Couldn't find files matching the required query.")

    # Markdownlint
    if "markdownlint" not in args.disabled_linters:
        markdownlint_suffix_list: Set = {".md"}
        markdownlint_cmd: List[str] = [
            "npx",
            "markdownlint",
            ".",
        ]
        markdownlint_filtered_file_list: List[str] = filter_files(
            file_list, markdownlint_suffix_list
        )
        print()
        print(" Markdown ".center(79, "="))

        if markdownlint_filtered_file_list:
            markdownlint_cmd.extend(markdownlint_filtered_file_list)
            print(
                "Checking markdown files format with Markdownlint: "
                f"{markdownlint_filtered_file_list}"
            )

            try:
                perform_linting(repo_root, markdownlint_cmd)
            except FormatError:
                exit_error = True
            else:
                print("Markdownlint done. No format issues.")
        else:
            print("No markdown files changed.")

    # Clang-format
    if "clang-format" not in args.disabled_linters:
        clang_format_suffix_list: Set = {
            ".h",
            ".hxx",
            ".c",
            ".cxx",
            ".java",
        }
        clang_format_cmd: List[str]

        if args.all_files:
            clang_format_cmd = [
                str(current_script_dir.joinpath("diff_clang_format.sh"))
            ]
        else:
            clang_format_cmd = [
                "git",
                "clang-format",
                "--style",
                "file",
                "--diff",
            ]

        clang_format_filtered_file_list: List[str] = filter_files(
            file_list, clang_format_suffix_list
        )
        print()
        print(" C/C++, C# & Java ".center(79, "="))

        if clang_format_filtered_file_list:
            if commits:
                clang_format_cmd.extend(commits)

            clang_format_cmd.append("--")
            clang_format_cmd.extend(clang_format_filtered_file_list)
            print(
                "Checking C/C++, C# & Java files format with clang-format: "
                f"{clang_format_filtered_file_list}"
            )

            try:
                perform_linting(repo_root, clang_format_cmd)
            except FormatError:
                exit_error = True
            else:
                print("clang-format done. No format issues.")
        else:
            print("No C/C++, C# or Java files changed.")

    # Black
    if "black" not in args.disabled_linters:
        black_suffix_list: Set = {".py"}
        black_cmd: List[str] = ["black", "--check", "--diff", "-l", "79"]
        black_filtered_file_list: List[str] = filter_files(
            file_list, black_suffix_list
        )
        print()
        print(" Python ".center(79, "="))

        if black_filtered_file_list:
            black_cmd.extend(black_filtered_file_list)
            print(
                "Checking Python files format with Black: "
                f"{black_filtered_file_list}"
            )

            try:
                perform_linting(repo_root, black_cmd)
            except FormatError:
                exit_error = True
            else:
                print("Black done. No format issues.")
        else:
            print("No Python files changed.")

    sys.exit(exit_error)
