# Example Code: Built-in QoS Profiles

## Building C++ Example
Before compiling or running the example, make sure the environment variable
`NDDSHOME` is set to the directory where your version of *RTI Connext* is
installed.

Run *rtiddsgen* with the `-example` option and the target architecture of your
choice (e.g., *i86Win32VS2010* or *i86Linux2.6gcc4.4.5*). The *RTI Connext Core
Libraries and Utilities Getting Started Guide* describes this process in detail.
Follow the same procedure to generate the code and build the examples. **Do not
use the `-replace` option.** Assuming you want to generate an example for
*i86Win32VS2010* run:
```
rtiddsgen -language C++ -example i86Win32VS2010 profiles.idl
```

You will see messages that look like this:
```
File C:\local\builtin_qos_profiles\c++\profiles_subscriber.cxx already exists
and will not be replaced with updated content. If you would like to get a new
file with the new content, either remove this file or supply -replace option.
File C:\local\ builtin_qos_profiles\c++\profiles_publisher.cxx already exists
and will not be replaced with updated content. If you would like to get a new
file with the new content, either remove this file or supply -replace option.
```

This is normal and is only informing you that the subscriber/publisher code has
not been replaced, which is fine since all the source files for the example are
already provided.

## Link and Run with Dynamic Libraries
This example dynamically loads the *RTI Monitoring library*. If you statically
link with the other RTI libraries, this will fail at runtime.

To dynamically link with the RTI libraries on *Windows*, choose the *Debug DLL*
or *Release DLL* build target. Make sure that RTI's libraries are in the system
`PATH` environment variable, such as:
`PATH=c:\Program Files\rti_connext_dds-5.2.0\lib\i86Win32VS2010;...`

To dynamically link with the RTI libraries on *Linux*, modify the makefile to
remove the 'z' from the end of the RTI library names, such as:
`-lnddscpp -lnddsc -lnddscore`

Also, make sure that RTI's libraries are in the system `LD_LIBRARY_PATH`
environment variable, such as:
`export LD_LIBRARY_PATH=~/rti_connext_dds-5.2.0/lib/i86Linux2.6gcc4.4.5:$LD_LIBRARY_PATH`

## Running C++ Example
In two separate command prompt windows for the publisher and subscriber. Run
the following commands from the example directory (this is necessary to ensure
the application loads the QoS defined in *USER_QOS_PROFILES.xml*):

On *Windows* systems run:
```
objs\<arch_name>\profiles_publisher.exe <domain_id> <samples_to_send>
objs\<arch_name>\profiles_publisher.exe <domain_id> <sleep_periods>
```

On *UNIX* systems run:
```
./objs/<arch_name>/profiles_publisher <domain_id> <samples_to_send>
./objs/<arch_name>/profiles_publisher <domain_id> <sleep_periods>
```

The applications accept two arguments:

1. The `<domain_id>`. Both applications must use the same domain ID in order to
communicate. The default is 0.
2. How long the examples should run, measured in samples for the publisher
and sleep periods for the subscriber. A value of '0' instructs the
application to run forever; this is the default.
