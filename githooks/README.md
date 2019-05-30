# Git hook for local pre-commit linting

This hook will execute
[clang-format](https://clang.llvm.org/docs/ClangFormat.html) and
[markdownlint](https://github.com/igorshubovych/markdownlint-cli) whenever you
try to commit new changes, failing to commit if there is any format issues in
staged files.

## Installing the hook

Firstly, you have to copy pre-commit hook in this folder to the hooks folder:

```bash
cd <project_folder>
cp githooks/pre-commit .git/hooks/pre-commit
```

## clang-format

Checkout [llvm build page](http://llvm.org/builds/) for info about downloading
`clang-format`. Version 9 of clang-format must be installed along with it's git
integration.

## markdownlint

Version 6 of [NodeJS](https://nodejs.org/) is needed to use this linter.
To install last version of `markdownlint`:

```bash
npm install -g markdownlint-cli
```
