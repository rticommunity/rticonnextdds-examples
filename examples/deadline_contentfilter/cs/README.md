# Example Code: Deadlines and Content Filtering

## Building C# Example
Before compiling or running the example, make sure the environment variable
`NDDSHOME` is set to the directory where your version of *RTI Connext* is
installed.

Run *rtiddsgen* with the `-example` option and the target architecture of your
choice (e.g., *x64Win64VS2013*). The *RTI Connext Core Libraries and Utilities
Getting Started Guide* describes this process in detail. Follow the same
procedure to generate the code and build the examples. **Do not use the
`-replace` option.** Assuming you want to generate an example for
*x64Win64VS2013* run:
```
rtiddsgen -language c# -example x64Win64VS2013 -ppDisable deadline_contentfilter.idl
```

**Note**: If you are using *Visual Studio Express* add the `-express` option to
the command, i.e.:
```
rtiddsgen -language c# -example x64Win64VS2013 -express -ppDisable deadline_contentfilter.idc
```

You will see messages that look like:
```
WARN com.rti.ndds.nddsgen.emitters.FileEmitter File exists and will not be
overwritten : /some/path/deadline_contentfilter_subscriber.cs
WARN com.rti.ndds.nddsgen.emitters.FileEmitter File exists and will not be
overwritten : /some/path/deadline_contentfilter_publisher.cs
```

This is normal and is only informing you that the subscriber/publisher code has
not been replaced, which is fine since all the source files for the example are
already provided.

*rtiddsgen* generates one solution with three *Visual Studio* projects.
The project *deadline_contentfilter_type-dotnet4.0.vcxproj* contains the types of the
example and it should be compiled with *Visual Studio C++*. The other two
projects contain the C# publisher and subscriber example and should be compiled
with *Visual Studio C#*.

## Running C# Example
In two separate command prompt windows for the publisher and subscriber. Run
the following commands from the example directory (this is necessary to ensure
the application loads the QoS defined in *USER_QOS_PROFILES.xml*):

```
bin\<build_type>\deadline_contentfilter_publisher.exe  <domain_id> <samples_to_send>
bin\<build_type>\deadline_contentfilter_subscriber.exe <domain_id> <sleep_periods>
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
