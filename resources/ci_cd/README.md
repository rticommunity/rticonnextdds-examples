# Use full automation scripts

The scripts contained in this directory are primarily intended for CI/CD
services, as Travis-CI or GitHub Actions. As a summary, we currently have the
following files:

-   `linux_format.py`: allows to check the linting of various files within the
    repository: Python, Markdown and C/C++ source code files.

-   `linux_install.py`: performs an installation of a minimun RTI ConnextDDS
    package to build and perform static analysis of the examples.

-   `linux_static_analysis.py`: run static analysis after building the examples.

-   `diff_clang_format.sh`: this is a helper script used in `linux_format.py` to
    run diffs between the source code and the `clang-format`'s changes to the same
    source code.

To be able to run `linux_static_analysis.py` script you will need to install the
[*Sultan*] and [*scan-build*] Python packages.

For `linux_format.py` apart from [*Sultan*] you will also need the [*Black*]
Python package, [*clang-format*] tool and the [*markdownlint-cli*] utility for
the `linux_format.py` script.

[*Sultan*]: https://pypi.org/project/sultan/
[*scan-build*]: https://pypi.org/project/scan-build/
[*clang-format*]: https://pypi.org/project/clang-format/9.0.0/
[*Black*]: https://pypi.org/project/black/
[*markdownlint-cli*]: https://www.npmjs.com/package/markdownlint-cli/v/0.23.1
