# RTI Connext Examples

This repository includes:

- Code examples on how to use specific Connext features (under [examples/](./examples))
- Supporting code for learning materials (under [tutorials/](./tutorials))

## Cloning the repository

To clone the repository you will need to run `git clone` as follows to download
both the repository and its submodule dependencies:

```bash
git clone --recurse-submodule https://github.com/rticommunity/rticonnextdds-examples.git
```

If you forget to clone the repository with `--recurse-submodule`, simply run
the following command to pull all the dependencies:

```bash
git submodule update --init --recursive
```

## Versioning

The examples contained in this branch have been built and tested against **RTI Connext
7.4.0 EAR**. If you need examples that have been built and tested against older
versions of RTI Connext DDS, please check out **master** or the appropriate branch:

- [release/7.3.0](https://github.com/rticommunity/rticonnextdds-examples/tree/release/7.3.0)
- [release/7.2.0](https://github.com/rticommunity/rticonnextdds-examples/tree/release/7.2.0)
- [release/7.1.0](https://github.com/rticommunity/rticonnextdds-examples/tree/release/7.1.0)
- [release/7.0.0](https://github.com/rticommunity/rticonnextdds-examples/tree/release/7.0.0)
- [release/6.1.1](https://github.com/rticommunity/rticonnextdds-examples/tree/release/6.1.1)
- [release/6.1.0](https://github.com/rticommunity/rticonnextdds-examples/tree/release/6.1.0)
- [release/6.0.1](https://github.com/rticommunity/rticonnextdds-examples/tree/release/6.0.1)
- [release/6.0.0](https://github.com/rticommunity/rticonnextdds-examples/tree/release/6.0.0)
- [release/5.3.0](https://github.com/rticommunity/rticonnextdds-examples/tree/release/5.3.0)
- [release/5.2.0](https://github.com/rticommunity/rticonnextdds-examples/tree/release/5.2.0)
- [release/5.1.0](https://github.com/rticommunity/rticonnextdds-examples/tree/release/5.1.0)
- [release/5.0.0](https://github.com/rticommunity/rticonnextdds-examples/tree/release/5.0.0)

## How to contribute

To contribute enhancements or additional examples to the repository, follow the
instructions on our [RTI Connext DDS Examples
Wiki](https://github.com/rticommunity/rticonnextdds-examples/wiki). Your
contributions will be reviewed and as soon as they are approved they will
automatically be included in the [RTI Community Portal Examples
Section](http://community.rti.com). See [CONTRIBUTING.md](https://github.com/rticommunity/rticonnextdds-examples/blob/master/CONTRIBUTING.md)
for further information about how to contribute with new examples to this repository.
