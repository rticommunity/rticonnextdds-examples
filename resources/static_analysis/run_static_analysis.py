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
"""Perform static analysis.

TODO: MODULE DOCUMENTATION

Examples:
    TODO: SOME EXAMPLES
"""

import logging
import argparse
import subprocess

from pprint import pprint
from pathlib import Path
from subprocess import CalledProcessError, CompletedProcess

SCRIPT_PATH: Path = Path(__file__).resolve()

logger: logging.Logger = logging.getLogger(f"{SCRIPT_PATH.name}")
logger.setLevel(logging.DEBUG)

fh = logging.FileHandler(filename=SCRIPT_PATH.with_suffix(".log"))
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


def perform_static_analysis(tool, arguments):
    print(tool, ":", end="")
    pprint(arguments)


def main():
    parser = argparse.ArgumentParser(
        description=__doc__,
        allow_abbrev=False,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument(
        "-b",
        "--binary-dir",
        type=Path,
        required=True,
        metavar="path",
        help="sets binary dir path",
    )
    parser.add_argument(
        "-i",
        "--includes",
        type=str,
        nargs="+",
        metavar="path",
        help="sets custom includes",
    )
    parser.add_argument(
        "-d",
        "--defines",
        type=str,
        nargs="+",
        metavar="def",
        help="sets custom defines",
    )
    parser.add_argument(
        "-v",
        "--verbose",
        action="store_true",
        help="sets current run in verbose mode",
    )

    args = parser.parse_args()

    # TODO: Check if binary dir exists
    if not args.binary_dir.exists():
        pass
    src_include = str(args.binary_dir) + "/<curr_example>/<language>/src"
    system_includes = list(map(lambda x: f"-isystem{x}", args.includes))
    includes = list(map(lambda x: f"-I{x}", args.includes))
    defines = list(map(lambda x: f"-D{x}", args.defines))

    example_list = sorted(
        [
            example_binary_dir.name
            for example_binary_dir in args.binary_dir.iterdir()
            if (
                example_binary_dir.is_dir()
                and example_binary_dir.name != "CMakeFiles"
            )
        ]
    )

    # pprint(example_list)

    # TODO: Run each static analysis tool for each example (and each language
    # in it)
    static_analysis_tools = {
        "clang-tidy": {
            "sources": ["*.c", "*.cxx"],
            "options": ['-header-filter=".*"', "--"],
            "compilation_options": defines + system_includes,
            "includes_type": "-isystem",
        },
        "cppcheck": {
            "sources": ["*.c", "*.cxx"],
            "options": ["--suppress='*:*/ndds/*'", "--suppress='*:*/src/*'"],
            "compilation_options": defines + system_includes,
            "includes_type": "-I",
        },
    }

    for tool, argument_parts in static_analysis_tools.items():
        for example in example_list:
            print(example)
            # perform_static_analysis(tool, arguments)


if __name__ == "__main__":
    main()
