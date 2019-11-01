# Git hook for local pre-commit linting

This hook will execute
[clang-format](https://clang.llvm.org/docs/ClangFormat.html),
[markdownlint](https://github.com/igorshubovych/markdownlint-cli) and
[black](https://github.com/psf/black) whenever you try to commit new changes,
failing to commit if there is any format issues in staged files.

## Installing the hook

Firstly, you have to copy pre-commit hook in this folder to the hooks folder:

```bash
cd <project_folder>
cp resources/githooks/pre-commit .git/hooks/pre-commit
```

Then you need Python3.5+ installed in your system, and the python package
`Sultan`. After installing python and pip (python package manager), run the
following code in your terminal:

```bash
pip install --user --upgrade Sultan
```

## markdownlint

Version 6 of [NodeJS](https://nodejs.org/) is needed to use this linter.
To install last version of `markdownlint`:

```bash
npm install -g markdownlint-cli
```

## clang-format

Checkout [llvm build page](http://llvm.org/builds/) for info about downloading
`clang-format`. Version 9 of clang-format must be installed along with it's git
integration.

## black

Black is an uncompromising Python formatter. You can install it with pip:

```bash
pip install --user --upgrade black
```
