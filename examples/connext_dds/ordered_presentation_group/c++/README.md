# Example Code: Ordered Presentation Group

## Building C++ Example
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
rtiddsgen -language c++ -example x64Win64VS2013 ordered_group.idl
```

You will see messages that look like:
```
WARN com.rti.ndds.nddsgen.emitters.FileEmitter File exists and will not be
overwritten : /some/path/ordered_group_subscriber.cxx
WARN com.rti.ndds.nddsgen.emitters.FileEmitter File exists and will not be
overwritten : /some/path/ordered_group_publisher.cxx
```

This is normal and is only informing you that the subscriber/publisher code has
not been replaced, which is fine since all the source files for the example are
already provided.

Use the generated makefile of *Visual Studio* project to compile your
application.

## Running C++ Example
In two separate command prompt windows for the publisher and subscriber. Run
the following commands from the example directory (this is necessary to ensure
the application loads the QoS defined in *USER_QOS_PROFILES.xml*):

```sh
# Do not forget to replace the path separator to "\" on Windows.
objs/<arch_name>/ordered_group_publisher.exe  <domain_id> <samples_to_send>
objs/<arch_name>/ordered_group_subscriber.exe <domain_id> <sleep_periods>
```

The applications accept up to two arguments:

1. The `<domain_id>`. Both applications must use the same domain ID in order to
communicate. The default is 0.
2. How long the examples should run, measured in samples for the publisher
and sleep periods for the subscriber. A value of '0' instructs the
application to run forever; this is the default.

## Publisher Output
```
Writing ordered_group, count 0
Writing ordered_group, count 1
Writing ordered_group, count 2
Writing ordered_group, count 3
Writing ordered_group, count 4
Writing ordered_group, count 5
Writing ordered_group, count 6
Writing ordered_group, count 7
Writing ordered_group, count 8
Writing ordered_group, count 9
```

## Subscriber Output
```
ordered_group subscriber sleeping for 4 sec...

   message: "First sample, Topic 1 sent by DataWriter number 1"

   message: "Second sample, Topic 1 sent by DataWriter number 1"

   message: "First sample, Topic 2 sent by DataWriter number 2"

   message: "Second sample, Topic 2 sent by DataWriter number 2"

   message: "First sample, Topic 3 sent by DataWriter number 3"

   message: "Second sample, Topic 3 sent by DataWriter number 3"

   message: "First sample, Topic 1 sent by DataWriter number 1"

   message: "Second sample, Topic 1 sent by DataWriter number 1"

   message: "First sample, Topic 2 sent by DataWriter number 2"

   message: "Second sample, Topic 2 sent by DataWriter number 2"

   message: "First sample, Topic 3 sent by DataWriter number 3"

   message: "Second sample, Topic 3 sent by DataWriter number 3"
ordered_group subscriber sleeping for 4 sec...

   message: "First sample, Topic 1 sent by DataWriter number 1"

   message: "Second sample, Topic 1 sent by DataWriter number 1"

   message: "First sample, Topic 2 sent by DataWriter number 2"

   message: "Second sample, Topic 2 sent by DataWriter number 2"

   message: "First sample, Topic 3 sent by DataWriter number 3"

   message: "Second sample, Topic 3 sent by DataWriter number 3"

   message: "First sample, Topic 1 sent by DataWriter number 1"

   message: "Second sample, Topic 1 sent by DataWriter number 1"

   message: "First sample, Topic 2 sent by DataWriter number 2"

   message: "Second sample, Topic 2 sent by DataWriter number 2"

   message: "First sample, Topic 3 sent by DataWriter number 3"

   message: "Second sample, Topic 3 sent by DataWriter number 3"

   message: "First sample, Topic 1 sent by DataWriter number 1"

   message: "Second sample, Topic 1 sent by DataWriter number 1"

   message: "First sample, Topic 2 sent by DataWriter number 2"

   message: "Second sample, Topic 2 sent by DataWriter number 2"

   message: "First sample, Topic 3 sent by DataWriter number 3"

   message: "Second sample, Topic 3 sent by DataWriter number 3"

   message: "First sample, Topic 1 sent by DataWriter number 1"

   message: "Second sample, Topic 1 sent by DataWriter number 1"

   message: "First sample, Topic 2 sent by DataWriter number 2"

   message: "Second sample, Topic 2 sent by DataWriter number 2"

   message: "First sample, Topic 3 sent by DataWriter number 3"

   message: "Second sample, Topic 3 sent by DataWriter number 3"
ordered_group subscriber sleeping for 4 sec...

   message: "First sample, Topic 1 sent by DataWriter number 1"

   message: "Second sample, Topic 1 sent by DataWriter number 1"

   message: "First sample, Topic 2 sent by DataWriter number 2"

   message: "Second sample, Topic 2 sent by DataWriter number 2"

   message: "First sample, Topic 3 sent by DataWriter number 3"

   message: "Second sample, Topic 3 sent by DataWriter number 3"

   message: "First sample, Topic 1 sent by DataWriter number 1"

   message: "Second sample, Topic 1 sent by DataWriter number 1"

   message: "First sample, Topic 2 sent by DataWriter number 2"

   message: "Second sample, Topic 2 sent by DataWriter number 2"

   message: "First sample, Topic 3 sent by DataWriter number 3"

   message: "Second sample, Topic 3 sent by DataWriter number 3"

   message: "First sample, Topic 1 sent by DataWriter number 1"

   message: "Second sample, Topic 1 sent by DataWriter number 1"

   message: "First sample, Topic 2 sent by DataWriter number 2"

   message: "Second sample, Topic 2 sent by DataWriter number 2"

   message: "First sample, Topic 3 sent by DataWriter number 3"

   message: "Second sample, Topic 3 sent by DataWriter number 3"
```
