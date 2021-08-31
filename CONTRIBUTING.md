# Contributing to rticonnext-examples

## How to request a feature or enhancement

If there is a new feature or enhancement that you would like to add to
this repository, feel free to open an issue and we will take a look at it as
soon as possible. Do not forget to use a clear title and description
about your feature or enhancement that you would like us to include, and select
the correct template in the `GitHub` GUI.

## How to submit a change

To submit a change, open a Pull Request. Before submitting the Pull Request,
be sure to use the correct code style/format described in the following paragraphs.

To ensure the quality and consistency of the code, we use the
following linters:

-   For `Python` we use [black](https://pypi.org/project/black/) with a
    line length of 79.
-   For `C/C++`, `C#`, and `Java`, we use
    [clang-format](https://clang.llvm.org/docs/ClangFormat.html). The rules of
    formatting for this tool are defined in the `.clang-format` file.
-   For `Markdown` we use [markdown-cli](https://www.npmjs.com/package/markdownlint-cli),
    using the formatting rules defined in `.markdownlint.json`.

After you open the Pull Request, some automatic workflows will be started to
check the format/style of the changed documents. Another workflow will
be launched to build all of the examples to check if they have compilation errors.
(This workflow is launched in `GitHub Actions` if the Pull Request goes to the master
branch and in our internal `Jenkins` tool if the Pull Request goes to develop.)

The script located in `resources/ci_cd/linux_format.py` executes the linterns
in the automatic workflow. You can also execute this script on your local
machine. If you need help with how the script works, use
`python resources/ci_cd/linux_format.py --help`.

You can also build all of the examples in your local environment if you want to
check that everything works properly before opening the Pull Request. To do so,
follow these steps:

1.  Set the `RTI_INSTALLATION_PATH` environment variable. This variable should
    contain the path where the `rti_connext_dds-<version>` directory is located.
2.  Execute the `resources/ci_cd/linux_build.py` script. This script generates
    the build system using `CMake` and compiles all of the examples.
3.  Execute the `resources/ci_cd/linux_static_analysis.py` script. This script
    executes the static analysis on all of the examples. Before executing
    this script, install [this package](https://pypi.org/project/scan-build/).

## How to submit a new example

New examples should be submitted by RTI employees only, because they have access
to the unreleased builds. Before submitting a new example, follow these rules:

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

Note that your Pull Request will not be merged until at least two reviewers
have approved your changes and all the checks performed by the automatic workflows
pass. (Read the previous section, "How to submit a change", to learn more about
these workflows.)

## How to report bugs

Open an issue by selecting the bug report template in the `GitHub` GUI.
It would be helpful if you could provide us with the RTI product, version, and
system information (OS, compiler version, etc.). Also provide us with the current
behavior, the steps to reproduce the issue, and the expected behavior.

## Contributor License Agreement (CLA)

In order to accept your Pull Request, we need you to sign a [Contributor License
Agreement (CLA)](http://community.rti.com/cla). You only need to do this once;
we cross-check your GitHub username with the list of contributors who have
signed the CLA.
