# Contributing to rticonnext-examples

## How to request a feature or enhancement

If there is a new feature or enhancement that you would like to add to
this repository, feel free to open an issue and we will take a look at it as
soon as possible. Please, do not forget to use a clear title and description
about your feature or enhancement that you would like us to include.

## How to submit a change

If you have any change to submit do not hesitate in opening a Pull Request.
Before submitting the Pull Request, please be sure you are using the correct
code style/format that is explained in the following paragraphs.
code style/format.

To ensure the quality and consistency of the code, we use the
following linters:

-   `Python`:
[black](https://pypi.org/project/black/).
-   `C/C++`:
[clang-format](https://clang.llvm.org/docs/ClangFormat.html). The rules of
`clang-format` are listed in the `.clang-format` file.
-   `Markdown`: [markdown-cli](https://www.npmjs.com/package/markdownlint-cli)
using the rules listed in `.markdownlint.json`.

The script located in `resources/ci_cd/linux_format.py` executes the linterns
in the automatic workflow. You can also execute this script on your local
machine. If you need help with how the script works, use
`python resources/ci_cd/linux_format.py --help`.

## How to report bugs

You can use `Issues` for bug reporting using the correct template.
It would be helpful if you could provide us with the RTI Product, version, and
system information (such as OS, compiler version, etc...). Also, it is necessary
to provide us with the current behavior, the steps to reproduce the issue, and
what is the expected behavior.

## Contributor License Agreement (CLA)

To accept your pull request, we need you to sign a [Contributor License
Agreement (CLA)](http://community.rti.com/cla). You only need to do this once,
we cross-check your Github username with the list of contributors who have
signed the CLA.
