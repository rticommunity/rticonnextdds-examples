# Tutorial: Publish-Subscribe

This code is part of the Connext Publish-Subscribe tutorial.

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

## Running the Applications :rocket:

The applications are built under the `build` directory:

```sh
cd build
./home_automation_publisher
...
```

