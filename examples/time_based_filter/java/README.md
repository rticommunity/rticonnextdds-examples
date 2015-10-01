# Example Code: Time Based Filters

## Building Java Example
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
rtiddsgen -language java -example x64Win64VS2013 tbf.idl
```

You will see messages that look like:
```
WARN com.rti.ndds.nddsgen.emitters.FileEmitter File exists and will not be
overwritten : /some/path/tbfSubscriber.java
WARN com.rti.ndds.nddsgen.emitters.FileEmitter File exists and will not be
overwritten : /some/path/tbfPublisher.java
```

This is normal and is only informing you that the subscriber/publisher code has
not been replaced, which is fine since all the source files for the example are
already provided.

Before compiling in Java, make sure that the desired version of the *javac*
compiler is in your `PATH` environment variable.

To compile on *Windows* and *UNIX* you can use the generated makefile with
`make`, run `ant` or run `javac`:
``` sh
# Compile using make or gmake
make -f makefile_tbf_x64Linux3gcc4.8.2

# or using ant
ant

# or using javac. On UNIX use:
javac -cp .:$NDDSHOME/lib/java/nddsjava.jar *.java
# javac on Windows:
javac -cp .;%NDDSHOME%\lib\java\nddsjava.jar *.java
```

## Running Java Example
In two separate command prompt windows for the publisher and subscriber.
Run the following commands from the example directory (this is necessary to
ensure the application loads the QoS defined in *USER_QOS_PROFILES.xml*):

To run the application in *Windows* and *UNIX* you can either use the makefile
with `make`, run `ant` or run `java` command:
``` sh
# Run with make and the generated makefile
make ARGS="<domain_id> <samples_to_send>" -f makefile_tbf_x64Linux3gcc4.8.2 tbfPublisher
make ARGS="<domain_id> <sleep_periods>" -f makefile_tbf_x64Linux3gcc4.8.2 tbfSubscriber

# or using ant (it will run with default arguments, domain 0 and forever)
ant tbfPublisher
ant tbfSubscriber

# or using java. On UNIX use:
java -cp .:$NDDSHOME/lib/java/nddsjava.jar tbfPublisher <domain_id> <samples_to_send>
java -cp .:$NDDSHOME/lib/java/nddsjava.jar tbfSubscriber <domain_id> <sleep_periods>
# java on Windows:
java -cp .;%NDDSHOME%\lib\java\nddsjava.jar tbfPublisher <domain_id> <samples_to_send>
java -cp .;%NDDSHOME%\lib\java\nddsjava.jar tbfSubscriber <domain_id> <sleep_periods>
```

The applications accept up to two arguments:

1. The `<domain_id>`. Both applications must use the same domain ID in order to
communicate. The default is 0.
2. How long the examples should run, measured in samples for the publisher
and sleep periods for the subscriber. A value of '0' instructs the
application to run forever; this is the default.