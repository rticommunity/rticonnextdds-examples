# Example Code: Persistent Storage

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
rtiddsgen -language c++03 -example x64Win64VS2013 hello_world.idl
```

You will see messages that look like:
```
WARN com.rti.ndds.nddsgen.emitters.FileEmitter File exists and will not be
overwritten : /some/path/hello_world_subscriber.cxx
WARN com.rti.ndds.nddsgen.emitters.FileEmitter File exists and will not be
overwritten : /some/path/hello_world_publisher.cxx
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

**Note: If you are using *TimesTen*, you may need run the command prompt
as administrator. Otherwise, none of the scenarios will not run correctly.**

**For TimesTen users**: In addition, you need to uncomment some lines in the
*USER_QOS_PROFILES.xml* and *persistence_service_configuration.xml* to load the
correct driver (we assuming you have installed *TimesTen 11.2.1*). Refer to
those documents for further details.

* Durable Writer History scenario

```sh
# Do not forget to replace the path separator to "\" on Windows.
objs/<arch_name>/hello_world_subscriber
objs/<arch_name>/hello_world_publisher -sample_count 5 -initial_value 0 -dwh 1
objs/<arch_name>/hello_world_subscriber
objs/<arch_name>/hello_world_publisher -sample_count 5 -initial_value 5 -dwh 1
```

* Durable Reader State Scenario

```sh
# Do not forget to replace the path separator to "\" on Windows.
objs/<arch_name>/hello_world_subscriber.exe -drs 1
objs/<arch_name>/hello_world_publisher.exe -sample_count 5 -initial_value 0 -sleep 60
stop hello_world_subscriber
objs/<arch_name>/hello_world_subscriber.exe
objs/<arch_name>/hello_world_subscriber.exe -drs 1
```

* Persistence Service Scenario

```sh
# Do not forget to replace the path separator to "\" and
# "$NDDSHOME" by "%NDDSHOME%" on Windows.

# Run persistence service (in the same folder that there are
# persistence_service_configuration.xml):
"$NDDSHOME/bin/rtipersistenceservice" -cfgFile persistence_service_configuration.xml -cfgName <persistence_service_database|persistence_service_filesystem>

objs/<arch_name>/hello_world_subscriber
objs/<arch_name>/hello_world_publisher -sample_count 5 -initial_value 0
objs/<arch_name>/hello_world_subscriber
```

The applications accepts different arguments:
```
hello_world_subscriber:
    -domainId <domain ID> (default: 0)
    -sample_count <sample_count> (default: infinite => 0)
    -drs <1|0> Enable/Disable durable reader state (default: 0)

hello_world_publisher:
    -domainId <domain ID> (default: 0)
    -sample_count <number of published samples> (default: infinite)
    -initial_value <first sample value> (default: 0)
    -sleep <sleep time in seconds before finishing> (default: 0)
    -dwh <1|0> Enable/Disable durable writer history (default: 0)
```
