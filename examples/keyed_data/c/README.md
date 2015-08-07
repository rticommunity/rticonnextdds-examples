# Example Code: Keyed data

## Building C Example
Before compiling or running the example, make sure the environment variable
`NDDSHOME` is set to the directory where your version of *RTI Connext* is
installed.

Run *rtiddsgen* with the `-example` option and the target architecture of your
choice (e.g., *x64Win64VS2013* or *x64Linux3gcc4.8.2*). The *RTI Connext Core
Libraries and Utilities Getting Started Guide* describes this process in detail.
Follow the same procedure to generate the code and build the examples. **Do not
use the `-replace` option.** Assuming you want to generate an example for
*x64Win64VS2013* run:
```
rtiddsgen -language c -example x64Win64VS2013 keys.idl
```

You will see messages that look like:
```
WARN com.rti.ndds.nddsgen.emitters.FileEmitter File exists and will not be
overwritten : /some/path/keys_subscriber.c
WARN com.rti.ndds.nddsgen.emitters.FileEmitter File exists and will not be
overwritten : /some/path/keys_publisher.c
```

This is normal and is only informing you that the subscriber/publisher code has
not been replaced, which is fine since all the source files for the example are
already provided.

Use the generated makefile of *Visual Studio* project to compile your
application.

## Running C Example
In two separate command prompt windows for the publisher and subscriber. Run
the following commands from the example directory (this is necessary to ensure
the application loads the QoS defined in *USER_QOS_PROFILES.xml*):

```sh
# Do not forget to replace the path separator to "\" on Windows.
objs/<arch_name>/keys_publisher.exe  <domain_id> <samples_to_send>
objs/<arch_name>/keys_subscriber.exe <domain_id> <sleep_periods>
```

The applications accept up to three arguments:

1. The `<domain_id>`. Both applications must use the same domain ID in order to
communicate. The default is 0.
2. How long the examples should run, measured in samples for the publisher
and sleep periods for the subscriber. A value of '0' instructs the
application to run forever; this is the default.

## Publisher Output
```
Registering instance 0
Writing instance 0, x: 1000, y: 0
Writing instance 0, x: 1000, y: 1
Writing instance 0, x: 1000, y: 2
Writing instance 0, x: 1000, y: 3
Writing instance 0, x: 1000, y: 4
----Registering instance 1
----Registering instance 2
Writing instance 0, x: 1000, y: 5
Writing instance 1, x: 2000, y: 5
Writing instance 2, x: 3000, y: 5
Writing instance 0, x: 1000, y: 6
Writing instance 1, x: 2000, y: 6
Writing instance 2, x: 3000, y: 6
Writing instance 0, x: 1000, y: 7
Writing instance 1, x: 2000, y: 7
Writing instance 2, x: 3000, y: 7
Writing instance 0, x: 1000, y: 8
Writing instance 1, x: 2000, y: 8
Writing instance 2, x: 3000, y: 8
Writing instance 0, x: 1000, y: 9
Writing instance 1, x: 2000, y: 9
Writing instance 2, x: 3000, y: 9
----Unregistering instance 1
Writing instance 0, x: 1000, y: 10
Writing instance 2, x: 3000, y: 10
Writing instance 0, x: 1000, y: 11
Writing instance 2, x: 3000, y: 11
Writing instance 0, x: 1000, y: 12
Writing instance 2, x: 3000, y: 12
Writing instance 0, x: 1000, y: 13
Writing instance 2, x: 3000, y: 13
Writing instance 0, x: 1000, y: 14
Writing instance 2, x: 3000, y: 14
----Disposing instance 2
Writing instance 0, x: 1000, y: 15
```

## Subscriber Output
```
Found new instance; code = 0
Instance 0: x: 1000, y: 0
Instance 0: x: 1000, y: 1
Instance 0: x: 1000, y: 2
Instance 0: x: 1000, y: 3
Instance 0: x: 1000, y: 4
Instance 0: x: 1000, y: 5
Found new instance; code = 1
Instance 1: x: 2000, y: 5
Found new instance; code = 2
Instance 2: x: 3000, y: 5
Instance 0: x: 1000, y: 6
Instance 1: x: 2000, y: 6
Instance 2: x: 3000, y: 6
Instance 0: x: 1000, y: 7
Instance 1: x: 2000, y: 7
Instance 2: x: 3000, y: 7
Instance 0: x: 1000, y: 8
Instance 1: x: 2000, y: 8
Instance 2: x: 3000, y: 8
Instance 0: x: 1000, y: 9
Instance 1: x: 2000, y: 9
Instance 2: x: 3000, y: 9
Instance 1 has no writers
Instance 0: x: 1000, y: 10
Instance 2: x: 3000, y: 10
Instance 0: x: 1000, y: 11
Instance 2: x: 3000, y: 11
Instance 0: x: 1000, y: 12
Instance 2: x: 3000, y: 12
Instance 0: x: 1000, y: 13
Instance 2: x: 3000, y: 13
Instance 0: x: 1000, y: 14
Instance 2: x: 3000, y: 14
Instance 2 disposed
Instance 0: x: 1000, y: 15
Instance 0 has no writers
```
