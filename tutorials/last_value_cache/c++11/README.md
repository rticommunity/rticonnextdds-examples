# Tutorial: Last-value cache

This code is part of the Connext Last-value cache tutorial.

## Building the Example :wrench:

You can build the example following the instructions in the tutorial, or you can
build it using CMake as follows.

1. Generate the build files:

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

2. Build the applications:

```sh
cmake --build .
```

## Running the Example :rocket:

See the tutorial for instructions.
