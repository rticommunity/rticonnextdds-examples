# Use full automation scripts

The scripts contained in this directory are primarily intended for CI/CD
services, as Jenkins or GitHub Actions. As a summary, we currently have the
following files:

-   `linux_format.py`: allows to check the linting of various files within the
    repository: Python, Markdown and C/C++ source code files.

-   `linux_install.py`: performs an installation of a minimun RTI ConnextDDS
    package to build and perform static analysis of the examples.

-   `linux_build.py`: build all the `C/C++` examples using `CMake`.

-   `linux_static_analysis.py`: run static analysis. This should be called
    after building all the examples.

-   `jenkins_output.py`: generate a `Markdown` file from the template located
    in `resources/markdown_templates/JenkinsTemplate.md` which contains the
    details text for the GitHub check.

-   `diff_clang_format.sh`: this is a helper script used in `linux_format.py`
    to run diffs between the source code and the `clang-format`'s changes to
    the same source code.

For `linux_format.py` you will also need the [*Black*] Python package,
[*clang-format*] tool and the [*markdownlint-cli*] utility.

[*scan-build*]: https://pypi.org/project/scan-build/
[*clang-format*]: https://pypi.org/project/clang-format/9.0.0/
[*Black*]: https://pypi.org/project/black/
[*markdownlint-cli*]: https://www.npmjs.com/package/markdownlint-cli/v/0.23.1
