# Example Code: Deadlines and Content Filtering

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
rtiddsgen -language java -example x64Win64VS2013 deadline_contentfilter.idl
```

You will see messages that look like:
```
WARN com.rti.ndds.nddsgen.emitters.FileEmitter File exists and will not be
overwritten : /some/path/deadline_contentfilterSubscriber.java
WARN com.rti.ndds.nddsgen.emitters.FileEmitter File exists and will not be
overwritten : /some/path/deadline_contentfilterPublisher.java
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
make -f makefile_deadline_contentfilter_x64Linux3gcc4.8.2

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
make ARGS="<domain_id> <samples_to_send>" -f makefile_deadline_contentfilter_x64Linux3gcc4.8.2 deadline_contentfilterPublisher
make ARGS="<domain_id> <sleep_periods>" -f makefile_deadline_contentfilter_x64Linux3gcc4.8.2 deadline_contentfilterSubscriber

# or using ant (it will run with default arguments, domain 0 and forever)
ant deadline_contentfilterPublisher
ant deadline_contentfilterSubscriber

# or using java. On UNIX use:
java -cp .:$NDDSHOME/lib/java/nddsjava.jar deadline_contentfilterPublisher <domain_id> <samples_to_send>
java -cp .:$NDDSHOME/lib/java/nddsjava.jar deadline_contentfilterSubscriber <domain_id> <sleep_periods>
# java on Windows:
java -cp .;%NDDSHOME%\lib\java\nddsjava.jar deadline_contentfilterPublisher <domain_id> <samples_to_send>
java -cp .;%NDDSHOME%\lib\java\nddsjava.jar deadline_contentfilterSubscriber <domain_id> <sleep_periods>
```

The applications accept up to two arguments:

1. The `<domain_id>`. Both applications must use the same domain ID in order to
communicate. The default is 0.
2. How long the examples should run, measured in samples for the publisher
and sleep periods for the subscriber. A value of '0' instructs the
application to run forever; this is the default.

## Publisher Output
```
Writing instance0, x = 1, y = 1
Writing instance1, x = 1, y = 1
Writing instance0, x = 2, y = 2
Writing instance1, x = 2, y = 2
Writing instance0, x = 3, y = 3
Writing instance1, x = 3, y = 3
Writing instance0, x = 4, y = 4
Writing instance1, x = 4, y = 4
Writing instance0, x = 5, y = 5
Writing instance1, x = 5, y = 5
Writing instance0, x = 6, y = 6
Writing instance1, x = 6, y = 6
Writing instance0, x = 7, y = 7
Writing instance1, x = 7, y = 7
Writing instance0, x = 8, y = 8
Writing instance1, x = 8, y = 8
Writing instance0, x = 9, y = 9
Stopping writes to instance1
Offered deadline missed on instance code = 1
Writing instance0, x = 10, y = 10
Writing instance0, x = 11, y = 11
Offered deadline missed on instance code = 1
Writing instance0, x = 12, y = 12
Offered deadline missed on instance code = 1
Writing instance0, x = 13, y = 13
```

## Subscriber Output
```
@ t=2.59s, Instance0: <1,1>
@ t=2.61s, Instance1: <1,1>
@ t=3.59s, Instance0: <2,2>
@ t=3.59s, Instance1: <2,2>
@ t=4.59s, Instance0: <3,3>
@ t=4.59s, Instance1: <3,3>
@ t=5.59s, Instance0: <4,4>
@ t=5.59s, Instance1: <4,4>
@ t=6.59s, Instance0: <5,5>
@ t=6.59s, Instance1: <5,5>
Starting to filter out instance1
@ t=7.59s, Instance0: <6,6>
@ t=8.59s, Instance0: <7,7>
Missed deadline @ t=8.61s on instance code = 1
@ t=9.59s, Instance0: <8,8>
@ t=10.59s, Instance0: <9,9>
Missed deadline @ t=10.61s on instance code = 1
@ t=11.59s, Instance0: <10,10>
@ t=12.59s, Instance0: <11,11>
Missed deadline @ t=12.61s on instance code = 1
@ t=13.59s, Instance0: <12,12>
@ t=14.59s, Instance0: <13,13>
Missed deadline @ t=14.61s on instance code = 1
```
