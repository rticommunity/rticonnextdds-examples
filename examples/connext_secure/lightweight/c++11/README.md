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

## Running the example

This example is based on a standard rtiddsgen publisher and subscriber example
code. The code has been modified so that 2 topics are used instead of one.
The publisher and one of the subscribers use full security plugins, whereas the
other subscriber uses lightweight security. The Governance file used showcases
a configuration that is compatible with Lightweight security. However, one of
the topics uses a data_protection_kind ENCRYPT topic rule, which breaks
compatibility.

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

The lightweight profile uses the OpenSSL PSL.

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

## Notes on Lightweight Platform Support Library (PSL) configuration

The lightweight participant profiles now include explicit PSL configuration
properties for OpenSSL Cryptographic Library:

- OpenSSL profile (`lightweight_library::peer`):
  - `com.rti.serv.secure.psl_library=psl_openssl3`
  - `com.rti.serv.secure.psl_get_interface=PSL_OSSL_get_interface`
  - `com.rti.serv.secure.psl_get_configuration=PSL_OSSL_get_default_configuration`

This mirrors the newer lightweight security initialization model where PSL abstracts
cryptography library and enables easy switch between different cryptographic libraries
with properties.

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

### Application does not start because of linking errors

If your application does not start because of errors related to library linking
or missing symbols, make sure your LD_LIBRARY_PATH contains `lib` directory within
Connext installation. Check if the `lib` directory contains PSL library matching
the value of `psl_library` property and make sure `psl_get_interface` and
`psl_get_configuration` match the functions provided by this library.
