#!/usr/bin/env python3.7
import sys
import time
import datetime

from pathlib import Path
from sultan.api import Sultan


def main():
    time_script_start = time.perf_counter()
    examples_dir = Path("examples/connext_dds").resolve()
    build_dir = examples_dir.joinpath("build")
    cmake_build_all_path = Path(
        "resources/cmake/ConnextDdsBuildAllConfigurations.cmake"
    ).resolve()

    if not examples_dir.exists():

        sys.exit("Examples directory not found.")

    if not cmake_build_all_path.exists():

        sys.exit("Cmake script to build all configurations not found.")

    print("Building the examples...", flush=True)
    time_build_start = time.perf_counter()

    build_dir.mkdir(exist_ok=True)

    if not build_dir.exists():

        sys.exit("Build dir not found.")

    with Sultan.load(cwd=build_dir) as sultan:

        result = sultan.cmake(
            "-DBUILD_SHARED_LIBS=ON",
            "-DCMAKE_BUILD_TYPE=Release",
            examples_dir,
        ).run(halt_on_nonzero=False)

        if result.rc != 0:

            sys.exit("There was some errors during build.")

        result = sultan.intercept__build(
            "cmake", "--build .", "--config Release"
        ).run(halt_on_nonzero=False)

        if result.rc != 0:

            sys.exit("There was some errors during build.")

        time_build_end = time.perf_counter()

        print("Analyzing the build...", flush=True)
        time_analysis_start = time.perf_counter()

        result = sultan.analyze__build(
            "--verbose",
            "--status-bugs",
            "--exclude",
            Path.home().joinpath("rti_connext_dds-6.0.0/include"),
            "--exclude",
            ".",
            "-o",
            ".",
        ).run(halt_on_nonzero=False)

    time_analysis_end = time.perf_counter()
    time_script_end = time.perf_counter()

    time_script_elapsed = datetime.timedelta(
        seconds=round(time_script_end - time_script_start)
    )
    time_build_elapsed = datetime.timedelta(
        seconds=round(time_build_end - time_build_start)
    )
    time_analysis_elapsed = datetime.timedelta(
        seconds=round(time_analysis_end - time_analysis_start)
    )

    print(
        f"Total:    {time_script_elapsed}\n"
        f"Build:    {time_build_elapsed}\n"
        f"Analysis: {time_analysis_elapsed}"
    )

    sys.exit(result.rc)


if __name__ == "__main__":
    main()
