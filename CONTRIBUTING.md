# Contributing to rticonnext-examples

## How to request a feature or enhancement

If there is a new feature or enhancement that you would like to add to
this repository, feel free to open an issue and we will take a look at it as
soon as possible. Please, do not forget to use a clear title and description
about your feature or enhancement that you would like us to include and select
the correct template in the `GitHub` GUI.

## How to submit a change

If you have any change to submit do not hesitate in opening a Pull Request.
Before submitting the Pull Request, please be sure you are using the correct
code style/format that is explained in the following paragraphs.

To ensure the quality and consistency of the code, we use the
following linters:

-   For `Python` we are using [black](https://pypi.org/project/black/) with a
    line length of 79.
-   For `C/C++` and `Java` we are using
[clang-format](https://clang.llvm.org/docs/ClangFormat.html). The rules of
formatting for this tool are defined in the `.clang-format` file.
-   For `Markdown` we are using [markdown-cli](https://www.npmjs.com/package/markdownlint-cli)
using the formatting rules defined in `.markdownlint.json`.

The script located in `resources/ci_cd/linux_format.py` executes the linterns
in the automatic workflow. You can also execute this script on your local
machine. If you need help with how the script works, use
`python resources/ci_cd/linux_format.py --help`.

## How to submit a new example

Before submitting a new example, you have to follow the next rules:

-   Obviously, the example should work properly.
-   Make sure your code follows the code conventions (read the previous
section).
-   Use the `Markdown` templates to describe how to reproduce the build and
the execution of the example. The templates are located in
`resources/markdown_templates`.
-   Update the build system configuration files:
    -   If you added a new `C/C++` example for RTI Connext DDS, update the
    `CMakeList.txt` file located in `examples/connext_ds`.
    -   If you added a new `Java` example for RTI Connext DDS, update the
    `settings.gradle` file located in `examples/connext_ds`.

## How to report bugs

You can open an issue selecting the bug report template in the `GitHub` GUI.
It would be helpful if you could provide us with the RTI Product, version, and
system information (such as OS, compiler version, etc...). Also, it is necessary
to provide us with the current behavior, the steps to reproduce the issue, and
what is the expected behavior.

## Contributor License Agreement (CLA)

In order to accept your pull request, we need you to sign a [Contributor License
Agreement (CLA)](http://community.rti.com/cla). You only need to do this once,
we cross-check your Github username with the list of contributors who have
signed the CLA.
