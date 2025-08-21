# Example Code: Lightweight Security Interoperability

## Building the Example

Remember to set your environment variables with the script in your Connext
installation directory before building.

```sh
cd c++11/
mkdir build
cd build
cmake ..
cmake --build .
```

Note: The build process also copies USER_QOS_PROFILES.xml into the build
directory to ensure that it is loaded when you run the examples within the
build directory.

## Setting up Security artifacts

The build process copies the security folder with .cnf files into the build
directory. Use the provided python script to initialize them. This means that
`build/security` will contain all the security artifacts needed to run this example.

```sh
cd security
python3 setup_security.py
```

## Running the example

This example is based on a standard rtiddsgen publisher and subscriber example
code. The code has been modified so that 2 topics are used instead of one.
The publisher and one of the subscribers use full security plugins, whereas the
other subscriber uses lightweight security. The Governance file used showcases
a configuration that is compatible with Lightweight security. However, one of
the topics uses a data_protection_kind ENCRYPT topic rule, which breaks compatibility.

Run one instance of the subscriber without any CLI arguments.
This will use full security by default.

```sh
./Lws_subscriber
```

In a separate window, launch another subscriber with the --lw CLI argument,
which will create the lightweight subscriber.

```sh
./Lws_subscriber -lw
```

Then, in a third window, launch the publisher.

```sh
./Lws_publisher
```

The subscriber with full security will receive data from both topics, like so:

```sh
[color: BLUE, x: 9, y: 9, shapesize: 9]
Lws subscriber sleeping up to 1 sec...
[color: RED, x: 9, y: 9, shapesize: 9]
Lws subscriber sleeping up to 1 sec...
[color: BLUE, x: 10, y: 10, shapesize: 10]
Lws subscriber sleeping up to 1 sec...
[color: RED, x: 10, y: 10, shapesize: 10]
```

The subscriber with lightweight security will only receive blue data.

```sh
[color: BLUE, x: 9, y: 9, shapesize: 9]
Lws subscriber sleeping up to 1 sec...
[color: BLUE, x: 10, y: 10, shapesize: 10]
Lws subscriber sleeping up to 1 sec...
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
