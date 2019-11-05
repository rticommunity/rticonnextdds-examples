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

        ./linux_format.py -r 8c0859a 487df86

    Disable on linter::

        ./linux_format.py -c 8c0859a --disable-markdownlint

"""
import sys
import time
import argparse

from pathlib import Path
from sultan.api import Sultan


class FormatError(Exception):
    pass


def get_argument_parser():
    """Create argument parser.

    Returns:
        The argument parser.

    """
    parser = argparse.ArgumentParser(
        description=__doc__,
        allow_abbrev=False,
        formatter_class=argparse.RawDescriptionHelpFormatter,
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
        "-cxx",
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


def stream_sultan_output(result):
    """Stream command output.

    Args:
        result (sultan.result.Result): Command result object.

    """
    while True:
        complete = result.is_complete

        for line in result.stdout:
            print(line)

        for line in result.stderr:
            print(line)

        if complete:
            break

        time.sleep(1)


def get_changed_files(repo_root, commits=None):
    """Obtains changed files.

    Args:
        repo_root (Path): Repository top folder.
        first_commit_hash: First commit hash. Defaults to None.
        second_commit_hash: Second commit hash. Defaults to None.

    Returns:
        List[str]: List of staged files.

    """
    command = ["diff", "--no-commit-id", "--name-only", "-r"]

    if commits:
        command.extend([*commits])
    else:
        command.insert(3, "--cached")
        command.append("HEAD")

    with Sultan.load(cwd=repo_root) as s:
        changed_files = (
            s.git(*command).run(halt_on_nonzero=False, quiet=True).stdout
        )

    return changed_files


def filter_files(file_list, extensions):
    """Filter files with selected extensions.

    Args:
        file_list (List[str]): List of files.
        extensions (Set[str], Optional): Set of extensions. Defaults to empty
            set.

    Returns:
        List[str]: Filtered list of files.

    """
    return [
        current_file
        for current_file in file_list
        if Path(current_file).suffix in extensions
    ]


def perform_linting(repo_root, command):
    """Performs ``command`` linter to ``files``.

    Args:
        repo_root (Path): Repository top folder.
        command (List[str]): List of commands to run the linter.
        file_list (List[str]): List of files that will be lintered.

    Raises:
        FormatError: When the linter outputs format errors.

    """
    with Sultan.load(cwd=repo_root) as s:
        lint_result = getattr(s, command[0])(*command[1:]).run(
            halt_on_nonzero=False, quiet=True
        )

    if lint_result.rc != 0 or len(lint_result.stdout) > 1:
        error_output = lint_result.stdout
        stream_sultan_output(lint_result)
        raise FormatError("Format error:\n{}".format(error_output))


if __name__ == "__main__":
    args = get_argument_parser().parse_args()
    script_path = Path(__file__)

    with Sultan.load(logging=False, cwd=script_path.parent) as s:
        result = s.git("rev-parse", "--show-toplevel").run(
            halt_on_nonzero=False, quiet=True
        )

        if result.rc != 0:
            sys.exit("Error: Script not in a git repository.")

        try:
            repo_root = Path(result.stdout[0]).resolve()
        except FileNotFoundError:
            sys.exit("Error: Root repo path not found.")
        else:
            if not repo_root.exists():
                sys.exit("Error: Root repo path not found.")

    commits = None

    if args.commit_range:
        commits = args.commit_range.split("...")
    elif args.commit:
        commits = [args.commit + "^", args.commit]

    exit_error = False
    file_list = get_changed_files(repo_root, commits)

    if not file_list:
        sys.exit()

    # Markdownlint
    if "markdownlint" not in args.disabled_linters:
        markdownlint_suffix_list = {".md"}
        markdownlint_cmd = ["markdownlint"]
        markdownlint_filtered_file_list = filter_files(
            file_list, markdownlint_suffix_list
        )

        if markdownlint_filtered_file_list:
            markdownlint_cmd.extend(markdownlint_filtered_file_list)

            try:
                print("markdownlint... ", end="")
                perform_linting(repo_root, markdownlint_cmd)
            except FormatError:
                exit_error = True
            else:
                print("-- Done", end="")

    # Clang-format
    if "clang-format" not in args.disabled_linters:
        clang_format_suffix_list = {".c", ".cxx"}
        clang_format_cmd = ["git", "clang-format", "--diff"]
        clang_format_filtered_file_list = filter_files(
            file_list, clang_format_suffix_list
        )

        if commits:
            clang_format_cmd.extend(commits)

        if clang_format_filtered_file_list:
            clang_format_cmd.append("--")
            clang_format_cmd.extend(clang_format_filtered_file_list)

            try:
                print("\nclang-format... ", end="")
                perform_linting(repo_root, clang_format_cmd)
            except FormatError:
                exit_error = True
            else:
                print("-- Done", end="")

    # Black
    if "black" not in args.disabled_linters:
        black_suffix_list = {".py"}
        black_cmd = ["black", "--check", "--diff", "-l", "79"]
        black_filtered_file_list = filter_files(file_list, black_suffix_list)

        if black_filtered_file_list:
            black_cmd.extend(black_filtered_file_list)

            try:
                print("\nblack... ", end="")
                perform_linting(repo_root, black_cmd)
            except FormatError:
                exit_error = True
            else:
                print("-- Done")

    sys.exit(exit_error)
