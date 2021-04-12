# Contributing to rticonnext-examples

## How to request a feature or enhacement

If there is a new feature or enhancement that you would like to add to
this repository, feel free to open an issue and we will take a look at it as
soon as possible. Please, do not forget to use a clear title and description
about your feature or enhancement that you would like us to include.

## How to submit a change

If you have any change to submit do not hesitate in opening a Pull Request.
Before you submit the Pull Request, please be sure you are using a correct
code style/format.

-   If you submit changes into a `Python` script or you are adding a new
one, be aware we will check the style/format with an automatic workflow. If you
are not sure you are using a correct style/format, install
[black](https://pypi.org/project/black/) and execute it in your local machine
to ensure the correctness of your code.

-   If you submit changes into a `C/C++` source file, you also have to use a
correct style/format of the code. For this purpose, we are using
[clang-format](https://clang.llvm.org/docs/ClangFormat.html). The rules of
`clan-format` are listed into the `.clang-format` file.

-   If you submit changes into a `Markdown` file, we are using some format rules
listed into `.markdownlint.json`. The tool we are using for checking the
`Markdown` rules is
[markdown-cli](https://www.npmjs.com/package/markdownlint-cli).

To execute the linters, we use a script located in
`resources/ci_cd/linux_format.py` in the automatic workflow. You can
also execute this script in your local machine.

```plaintext
Usage: python resources/ci_cd/linux_format.py [options]

Options:
 -r     <commit hash range>  Commits hash range on which the linters
                             will be executed
 -c     <commit hash>        Commit hash on which the linters will be
                             executed
 -a                          Run the linters for all the files
 -md                         Disable Markdown linter
 -cxx                        Disable clang-format linter
 -py                         Disable black linter

```

## How to report bugs

You can use the issues for bug reporting using the correct template.
It would be helpful if you could provide us with the RTI Product and system
information. Also, it is neccesary to provide us with the current behavior, the
steps to reproduce the issue and what is the expected behavior.

## Contributor License Agreement (CLA)

In order to accept your pull request, we need you to sign a [Contributor License
Agreement (CLA)](http://community.rti.com/cla). You only need to do this once,
we cross-check your Github username with the list of contributors who have
signed the CLA.
