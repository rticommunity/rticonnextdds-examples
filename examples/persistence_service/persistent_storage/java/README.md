# Example Code: Persistent Storage

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
rtiddsgen -language java -example x64Win64VS2013 hello_world.idl
```

You will see messages that look like:
```
WARN com.rti.ndds.nddsgen.emitters.FileEmitter File exists and will not be
overwritten : /some/path/hello_worldSubscriber.java
WARN com.rti.ndds.nddsgen.emitters.FileEmitter File exists and will not be
overwritten : /some/path/hello_worldPublisher.java
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
make -f makefile_hello_world_x64Linux3gcc4.8.2

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

**Note: If you are using *TimesTen*, you may need run the command prompt
as administrator. Otherwise, none of the scenarios will not run correctly.**

**For TimesTen users**: In addition, you need to uncomment some lines in the
*USER_QOS_PROFILES.xml* and *persistence_service_configuration.xml* to load the
correct driver (we assuming you have installed *TimesTen 11.2.1*). Refer to
those documents for further details.

* On *Windows* systems run:
    * *Durable Writer History scenario*:
      1. `java -cp .;%NDDSHOME%\lib\java\nddsjava.jar hello_worldSubscriber`
      2. `java -cp .;%NDDSHOME%\lib\java\nddsjava.jar hello_worldPublisher -sample_count 5 -initial_value 0 -dwh 1`
      3. `java -cp .;%NDDSHOME%\lib\java\nddsjava.jar hello_worldSubscriber`
      4. `java -cp .;%NDDSHOME%\lib\java\nddsjava.jar hello_worldPublisher -sample_count 5 -initial_value 5 -dwh 1`

    * *Durable Reader State Scenario*:
      1. `java -cp .;%NDDSHOME%\lib\java\nddsjava.jar hello_worldSubscriber -drs 1`
      2. `java -cp .;%NDDSHOME%\lib\java\nddsjava.jar hello_worldPublisher -sample_count 5 -initial_value 0 -sleep 60`
      3. stop *hello_worldSubscriber*
      4. `java -cp .;%NDDSHOME%\lib\java\nddsjava.jar hello_worldSubscriber`
      5. `java -cp .;%NDDSHOME%\lib\java\nddsjava.jar hello_worldSubscriber -drs 1`

    * *Persistence Service Scenario*:
      1. Run persistence service (in the same folder that there are
         *persistence_service_configuration.xml*):
         ```
         %NDDSHOME%\bin\rtipersistenceservice.bat -cfgFile persistence_service_configuration.xml -cfgName <persistence_service_database|persistence_service_filesystem>
         ```
      2. `java -cp .;%NDDSHOME%\lib\java\nddsjava.jar hello_worldSubscriber`
      3. `java -cp .;%NDDSHOME%\lib\java\nddsjava.jar hello_worldPublisher -sample_count 5 -initial_value 0`
      4. `java -cp .;%NDDSHOME%\lib\java\nddsjava.jar hello_worldSubscriber`

* On *UNIX* systems run:
    * *Durable Writer History scenario*:
      1. `java -cp .:$NDDSHOME/lib/java/nddsjava.jar hello_worldSubscriber`
      2. `java -cp .:$NDDSHOME/lib/java/nddsjava.jar hello_worldPublisher -sample_count 5 -initial_value 0 -dwh 1`
      3. `java -cp .:$NDDSHOME/lib/java/nddsjava.jar hello_worldSubscriber`
      4. `java -cp .:$NDDSHOME/lib/java/nddsjava.jar hello_worldPublisher -sample_count 5 -initial_value 5 -dwh 1`

    * *Durable Reader State Scenario*:
      1. `java -cp .:$NDDSHOME/lib/java/nddsjava.jar hello_worldSubscriber -drs 1`
      2. `java -cp .:$NDDSHOME/lib/java/nddsjava.jar hello_worldPublisher -sample_count 5 -initial_value 0 -sleep 60`
      3. stop *hello_worldSubscriber*
      4. `java -cp .:$NDDSHOME/lib/java/nddsjava.jar hello_worldSubscriber`
      5. `java -cp .:$NDDSHOME/lib/java/nddsjava.jar hello_worldSubscriber -drs 1`

    * *Persistence Service Scenario*:
      1. Run persistence service (in the same folder that there are
         *persistence_service_configuration.xml*):
         ```
         %NDDSHOME%\bin\rtipersistenceservice -cfgFile persistence_service_configuration.xml -cfgName <persistence_service_database|persistence_service_filesystem>
         ```
      2. `java -cp .:$NDDSHOME/lib/java/nddsjava.jar hello_worldSubscriber`
      3. `java -cp .:$NDDSHOME/lib/java/nddsjava.jar hello_worldPublisher -sample_count 5 -initial_value 0`
      4. `java -cp .:$NDDSHOME/lib/java/nddsjava.jar hello_worldSubscriber`

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
