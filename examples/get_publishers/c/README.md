# Example Code: get_publishers

## Building C Example
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
rtiddsgen -language C -example i86Win32VS2010 Foo.idl
```

You will see messages that look like this:
```
File C:\local\Foo\c\Foo_publisher.c already exists and will
not be replaced with updated content. If you would like to get a new file with
the new content, either remove this file or supply -replace option.
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

On *Windows* systems run:
```
objs\<arch_name>\Foo_publisher.exe <domain_id>
```

On *UNIX* systems run:
```
./objs/<arch_name>/Foo_publisher <domain_id>
```

The applications accept one argument:

1. The `<domain_id>`. Both applications must use the same domain ID in order to
communicate. The default is 0.
