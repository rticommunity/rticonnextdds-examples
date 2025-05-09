# Use full automation scripts

The scripts contained in this directory are primarily intended for GitHub
Actions CI. As a summary, we currently have the following files:

-   `linux_format.py`: allows to check the linting of various files within the
    repository: Python, Markdown and C/C++ source code files.

-   `diff_clang_format.sh`: this is a helper script used in `linux_format.py`
    to run diffs between the source code and the `clang-format`'s changes to
    the same source code.

For `linux_format.py` you will also need the [*Black*] Python package,
[*clang-format*] tool and the [*markdownlint-cli*] utility.

[*clang-format*]: https://pypi.org/project/clang-format/9.0.0/
[*Black*]: https://pypi.org/project/black/
[*markdownlint-cli*]: https://www.npmjs.com/package/markdownlint-cli/v/0.23.1
