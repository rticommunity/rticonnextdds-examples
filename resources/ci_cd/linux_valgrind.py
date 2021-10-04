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
"""Perform the Valgrind memcheck over all the built examples."""

import os
import sys
import json
import subprocess
from pathlib import Path
import concurrent.futures
from typing import List, Dict
from dataclasses import dataclass

ERROR_CODE = 1
LANG_DIRNAMES = {"c", "c++98", "c++11"}


@dataclass
class ValgrindConfig:
    """Class for parsing the JSON objects"""

    example_path: Path = None
    languages: Dict[str, Dict] = None
    skip: bool = False
    environment: Dict[str, str] = None
    metadata: Dict[str, str] = None

    @classmethod
    def parse(cls, config):
        """Parse the JSON file and initializes a ValgrindConfig object"""
        data = json.load(config)
        path = Path(config.name).parent

        valgrind_config = data["valgrind"]

        return ValgrindConfig(example_path=path, **valgrind_config)


@dataclass
class SubdirectoryConfig:
    """Class for parsing the JSON objects"""

    subdirectories: List[Path]

    @classmethod
    def parse(cls, config):
        """Parse the JSON file and initializes a SubdirectoryConfig object"""
        data = json.load(config)
        parent_directory = Path(config.name).parent

        subdirectories = map(
            lambda subdir: parent_directory.joinpath(subdir),
            data["valgrind"]["subdirectories"],
        )

        return SubdirectoryConfig(subdirectories)


def execute_application(
    exe_path: Path, config: Dict, arg_index: int, environment: Dict[str, str]
):
    """Execute Valgrind memcheck in ```executable_path```. It also detects if
    the example is implemented using the old template or the new template.

    :param executable_path: The path of the executable which will be checked
    with Valgrind.
    :returns: Valgrind return code.
    """
    command = []

    # If the timeout key is defined, set the timeout for the subscriber
    if "timeout" in config:
        command += ["timeout", "--preserve-status", str(config["timeout"])]

    # Valgrind command and options
    command += [
        "valgrind",
        "--leak-check=yes",
        "--error-exitcode=1",
        f"./{exe_path.parts[-1]}",
    ]

    # Add the executable arguments
    if "sub" in exe_path.parts[-1] and "sub_args" in config:
        command.extend(config["sub_args"][arg_index])
    elif "pub" in exe_path.parts[-1] and "pub_args" in config:
        command.extend(config["pub_args"][arg_index])

    print(f"Arguments: {command}", flush=True)

    # Run Valgrind and capture the output to not display the example logs.
    valgrind_result = subprocess.run(
        command, capture_output=True, env=environment, cwd=exe_path.parent
    )

    # Show Valgrind logs messages.
    print(valgrind_result.stderr.decode("utf-8"), flush=True)

    return valgrind_result.returncode


def is_exe(path: Path) -> bool:
    """Check if ```path``` is executable.

    :param path: The path that will be checked.
    :returns: ```True``` if ```path``` is executable and ```False``` otherwise.
    """
    return path.is_file() and os.access(path, os.X_OK) and path.suffix != ".so"


def get_exe_list(path: Path) -> List[Path]:
    """Retrieve the executable files located in the given ```path```.

    :param path: The path where this function will search the executable files.
    :returns: List of executable files.
    """
    exe_files = []

    for file in path.iterdir():
        if is_exe(file):
            exe_files.append(file)

    return exe_files


def parse_json():
    JSON_NAME = "ci.json"
    examples_dir = Path("examples/connext_dds")
    config_list = []

    # Create the examples directories list
    examples_subdirs = []

    # Search for the subdirectories which contains examples.
    for subdir in examples_dir.iterdir():
        if subdir.name == "build" or not subdir.is_dir():
            continue

        # If the example does not contain a ci.json file, exit with fail.
        try:
            file = open(subdir.joinpath(JSON_NAME), "r")
        except FileNotFoundError:
            sys.exit(
                f"Error: the example {subdir.name} does not contains a ci.json "
                "file."
            )

        # If a subdirectory contains more subdirectories with different examples
        # add the subdirectories of the subdirectory.
        try:
            data = SubdirectoryConfig.parse(file)
        except KeyError:
            examples_subdirs.append(subdir)
        else:
            examples_subdirs.extend(data.subdirectories)

    # Iter each example directory from examples/connext_dds
    for dir in examples_subdirs:
        try:
            file = open(dir.joinpath(JSON_NAME), "r")
        except FileNotFoundError:
            sys.exit(
                f"Error: the example {dir.name} does not contains a ci.json "
                "file."
            )

        try:
            data = ValgrindConfig.parse(file)
        except KeyError:
            sys.exit(
                f"The ci.json file of the example {dir.name} does not contain"
                " the valgrind key."
            )

        if data.skip:
            continue

        # Check if the ci.json contains the example_type key and contains a
        # correct value
        if data.metadata:
            if "example_type" in data.metadata:
                example_type = data.metadata["example_type"]
                if not example_type in {
                    "pub-sub",
                    "no-pub-sub",
                    "pub-sub-onefile",
                }:
                    sys.exit(
                        "The example_type key of the ci.json file from the "
                        f"example {dir.name} does not contain a correct value "
                        "in the example_type key."
                    )
            else:
                sys.exit(
                    f"The metadata key of the ci.json from the {dir.name} "
                    "example does not contain the example_type key."
                )
        else:
            sys.exit(
                f"The ci.json file of the example {dir.name} does not contain "
                "the metada key inside the valgrind key."
            )

        # Check if the languages or subdirectories mandatory key exists.
        if not data.languages:
            sys.exit(
                f"Error parsing the ci.json from {dir.name}: if the "
                "example_type is pub-sub or pub-sub-onefile, include the "
                "languages key inside valgrind."
            )

        keys = list(data.languages.keys())

        lang_subdirs = [
            lang_dir.name
            for lang_dir in dir.iterdir()
            if lang_dir.is_dir() and lang_dir.name in LANG_DIRNAMES
        ]

        if set(keys) != set(lang_subdirs):
            sys.exit(
                f"Error parsing the ci.json from {dir.name}: the configured "
                "directories names differ from the existing directories."
            )

        for lang_data in data.languages.values():
            if "pub_args" in lang_data:
                if all(isinstance(arg, str) for arg in lang_data["pub_args"]):
                    lang_data["pub_args"] = [lang_data["pub_args"]]
                elif all(
                    isinstance(arg, list) for arg in lang_data["pub_args"]
                ):
                    if "sub_args" in lang_data and len(
                        lang_data["pub_args"]
                    ) != len(lang_data["sub_args"]):
                        sys.exit(
                            f"Error parsing the ci.json from {dir.name}: the "
                            "pub_args array length should be equal to "
                            "sub_args length."
                        )
                else:
                    sys.exit(
                        f"Error parsing the ci.json from {dir.name}: the type "
                        "of pub_args should be list of str or list of lists."
                    )

            if "sub_args" in lang_data:
                if all(isinstance(arg, str) for arg in lang_data["sub_args"]):
                    lang_data["sub_args"] = [lang_data["sub_args"]]
                elif not all(
                    isinstance(arg, list) for arg in lang_data["sub_args"]
                ):
                    sys.exit(
                        f"Error parsing the ci.json from {dir.name}: the type "
                        "of sub_args should be list of str or list of lists."
                    )

        config_list.append(data)

    return config_list


def main():
    """The main function"""
    try:
        build_dir = Path("examples/connext_dds/build").resolve(strict=True)
    except FileNotFoundError:
        sys.exit(
            "Error: build directory not found, compile the examples before "
            "running this script."
        )

    config_list = parse_json()

    # Iterate over all the built examples directories and languages
    # subdirectories
    for config in config_list:
        for lang, lang_config in config.languages.items():
            index = config.example_path.parts.index("connext_dds") + 1
            lang_dir = build_dir.joinpath(
                *config.example_path.parts[index:], lang
            )
            exe_list = get_exe_list(lang_dir)

            if config.metadata["example_type"] == "pub-sub-onefile":
                exe_list.extend(exe_list)

            information_message = (
                f" Example: {lang_dir.parts[-2]}"
                f" & Language: {lang_dir.parts[-1].capitalize()} "
            )

            print(information_message.center(79, "="), flush=True)

            if "pub_args" in lang_config:
                arg_count = len(lang_config["pub_args"])
            else:
                arg_count = 1

            for i in range(arg_count):
                # Execute the executable files of a example language subdirectory
                # in different threads.
                with concurrent.futures.ThreadPoolExecutor() as executor:
                    future_list = []
                    return_codes = []

                    # Create the future thread
                    for exe_file in exe_list:
                        future = executor.submit(
                            execute_application,
                            exe_file,
                            lang_config,
                            i,
                            config.environment,
                        )
                        future_list.append(future)

                    # Wait for the result and block the Main Thread.
                    for future in future_list:
                        result = future.result()
                        return_codes.append(result)

                    # If one Thread returned an error code, stop the execution.
                    if ERROR_CODE in return_codes:
                        print(
                            f"Memory leak discovered in {lang_dir}\n",
                            flush=True,
                        )
                        sys.exit(1)


if __name__ == "__main__":
    main()
