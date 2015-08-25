# Example Code: Custom Flowcontroller

## Building C++03 Example
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
rtiddsgen -language c++03 -example x64Win64VS2013 cfc.idl
```

You will see messages that look like:
```
WARN com.rti.ndds.nddsgen.emitters.FileEmitter File exists and will not be
overwritten : /some/path/cfc_subscriber.cxx
WARN com.rti.ndds.nddsgen.emitters.FileEmitter File exists and will not be
overwritten : /some/path/cfc_publisher.cxx
```

This is normal and is only informing you that the subscriber/publisher code has
not been replaced, which is fine since all the source files for the example are
already provided.

Use the generated makefile of *Visual Studio* project to compile your
application.

## Running C++03 Example
In two separate command prompt windows for the publisher and subscriber. Run
the following commands from the example directory (this is necessary to ensure
the application loads the QoS defined in *USER_QOS_PROFILES.xml*):

```sh
# Do not forget to replace the path separator to "\" on Windows.
objs/<arch_name>/cfc_publisher.exe  <domain_id> <samples_to_send>
objs/<arch_name>/cfc_subscriber.exe <domain_id> <sleep_periods>
```

The applications accept up to two arguments:

1. The `<domain_id>`. Both applications must use the same domain ID in order to
communicate. The default is 0.
2. How long the examples should run, measured in samples for the publisher
and sleep periods for the subscriber. A value of '0' instructs the
application to run forever; this is the default.

## Publisher Output
```
Writing cfc, sample 0
Writing cfc, sample 1
Writing cfc, sample 2
Writing cfc, sample 3
Writing cfc, sample 4
Writing cfc, sample 5
Writing cfc, sample 6
Writing cfc, sample 7
Writing cfc, sample 8
Writing cfc, sample 9
Writing cfc, sample 10
Writing cfc, sample 11
Writing cfc, sample 12
Writing cfc, sample 13
Writing cfc, sample 14
Writing cfc, sample 15
Writing cfc, sample 16
Writing cfc, sample 17
Writing cfc, sample 18
Writing cfc, sample 19
Writing cfc, sample 20
Writing cfc, sample 21
Writing cfc, sample 22
Writing cfc, sample 23
Writing cfc, sample 24
Writing cfc, sample 25
Writing cfc, sample 26
Writing cfc, sample 27
Writing cfc, sample 28
Writing cfc, sample 29
```

## Subscriber Output
```
@ t=0.032710s, got x = 0
@ t=0.032841s, got x = 1
@ t=0.033193s, got x = 2
@ t=0.033250s, got x = 3
@ t=0.033609s, got x = 4
@ t=0.033661s, got x = 5
@ t=0.033793s, got x = 6
@ t=0.033848s, got x = 7
@ t=0.034092s, got x = 8
@ t=0.034146s, got x = 9
@ t=0.035744s, got x = 10
@ t=0.035796s, got x = 11
@ t=0.036038s, got x = 12
@ t=0.036092s, got x = 13
@ t=0.036389s, got x = 14
@ t=0.036464s, got x = 15
@ t=0.036760s, got x = 16
@ t=0.036837s, got x = 17
@ t=0.037029s, got x = 18
@ t=0.037117s, got x = 19
@ t=0.038716s, got x = 20
@ t=0.038770s, got x = 21
@ t=0.038981s, got x = 22
@ t=0.039077s, got x = 23
@ t=0.039297s, got x = 24
@ t=0.039404s, got x = 25
@ t=0.039906s, got x = 26
@ t=0.039999s, got x = 27
@ t=0.040254s, got x = 28
@ t=0.040313s, got x = 29
```
