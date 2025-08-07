# Tutorial: Discovery

This code is part of the Connext Discovery
tutorial.

## Building the Example :wrench:

You can build the example following the instructions in the tutorial, or you can
build it using CMake as follows.

1.  Generate the build files:

```sh
mkdir build
cd build
cmake ..
```

This command will try to find the location of your Connext installation. If it
can't find it, specify it with the ``-DCONNEXTDDS_DIR`` option, for example:

```sh
cmake -DCONNEXTDDS_DIR=/home/rti/rti_connext_dds-x.y.z ..
```

If you are compiling for Windows you may also need to specify the
[generator](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html),
and platform.

```sh
cmake .. -G "Visual Studio 17 2022" -A x64
```

2.  Build the applications:

```sh
cmake --build .
```

If you are using a multi-configuration generator, such as Visual Studio
solutions, you can specify the configuration mode to build as follows:

```sh
cmake --build . --config Release|Debug
```

## Running the Applications :rocket:

The applications are built under the `build` directory:

```sh
cd build
./home_automation_publisher
...
```


## Troubleshooting

### Compilation fails accessing struct field

If the code compilation fails with errors such as "reference to non-static member
function must be called" for code such as `my_sample.my_field = value` or
`value = my_sample.my_field` this means that the rtiddsgen version you are using
doesn't have the IDL4 C++ mapping enabled by default.

To fix it, upgrade your Connext version to 7.6+ or check out the branch for the
Connext version you're using, e.g.

```sh
git checkout release/7.3.0
```
