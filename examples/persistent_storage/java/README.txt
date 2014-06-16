===========================================
 Example Code -- Persistent Storage
===========================================

Building Java Example
=====================
Before compiling or running the example, make sure the environment variable 
NDDSHOME is set to the directory where your version of RTI Connext is installed.

Run rtiddsgen with the -example option and the target architecture of your 
choice (e.g., i86Win32jdk or i86Linux2.6gcc4.4.3jdk). The RTI Connext Core 
Libraries and Utilities Getting Started Guide describes this process in detail. 
Follow the same procedure to generate the code and build the examples. Do not 
use the -replace option.

On Windows systems run:

rtiddsgen -language Java -example i86Win32jdk hello_world.idl

On UNIX systems (assuming you want to generate an example for 
i86Linux2.6gcc4.4.3) run:

rtiddsgen -language Java -example i86Linux2.6gcc4.4.3jdk hello_world.idl

You will see messages that look like this:

File C:\local\persistence_storage\java\hello_worldSubscriber.java already 
exists and will not be replaced with updated content. If you would like to get
a new file with the new content, either remove this file or supply -replace
option.
File C:\local\persistence_storage\java\hello_worldPublisher.java already 
exists and will not be replaced with updated content. If you would like to get
a new file with the new content, either remove this file or supply -replace
option.
File C:\local\persistence_storage\java\USER_QOS_PRIFLES.xml already exists and
will not be replaced with updated content. If you would like to get a new file
with the new content, either remove this file or supply -replace option.

This is normal and is only informing you that the subscriber/publisher code has 
not been replaced, which is fine since all the source files for the example are 
already provided.

Before compiling in Java, make sure that the desired version of the javac 
compiler is in your PATH environment variable.

On Windows systems run:

javac -classpath .;%NDDSHOME%\class\nddsjava.jar *.java

On Unix systems (including Linux and MacOS X):

javac -classpath .:$NDDSHOME/class/nddsjava.jar *.java

Running C Example
=================
Before running, make sure that the native Java libraries on which RTI Connext
depends are in your environment path (or library path). To add Java libraries 
to your environment...

On Windows systems run: 
set PATH=%NDDSHOME%\lib\i86Win32jdk;%PATH%

On Unix systems except MacOS X (assuming you are using Bash) run:
export LD_LIBRARY_PATH=$NDDSHOME/lib/<platform_name>jdk:$LD_LIBRARY_PATH

On MacOSX (assuming your are using Bash) run:
export DYLD_LIBRARY_PATH=$NDDSHOME/lib/<platform_name>jdk:$DYLD_LIBRARY_PATH

Then, use two separate command prompts for publisher and subscriber. Run the 
following commands from the example directory (this is necessary to ensure the
application loads the QoS defined in USER_QOS_PROFILES.xml):

**** IF YOU ARE USING TIMESTEN, YOU MAY NEED RUN THE COMMAND PROMPT AS      ****
**** ADMINISTRATOR. OTHERWISE, NONE OF THE SCENARIOS WILL NOT RUN CORRECTLY ****

(For TimesTen users) In addition, you need to uncomment some lines in the 
USER_QOS_PROFILES.xml and persistence_service_configuration.xml to load the 
correct driver (we assuming you have installed TimesTen 11.2.1). Refer to those
documents for further details.

Run the scenarios as described in the presentation persistent_storage.pps

Scenarios commands:

** On Windows systems run:
    Durable Writer History scenario
    1) java -cp .;%NDDSHOME%\class\nddsjava.jar hello_worldSubscriber
    2) java -cp .;%NDDSHOME%\class\nddsjava.jar hello_worldPublisher -sample_count 5 -initial_value 0 -dwh 1 
    3) java -cp .;%NDDSHOME%\class\nddsjava.jar hello_worldSubscriber
    4) java -cp .;%NDDSHOME%\class\nddsjava.jar hello_worldPublisher -sample_count 5 -initial_value 5 -dwh 1

    Durable Reader State Scenario
    1) java -cp .;%NDDSHOME%\class\nddsjava.jar hello_worldSubscriber -drs 1
    2) java -cp .;%NDDSHOME%\class\nddsjava.jar hello_worldPublisher -sample_count 5 -initial_value 0 -sleep 60 
    3) stop hello_worldSubscriber
    4) java -cp .;%NDDSHOME%\class\nddsjava.jar hello_worldSubscriber
    5) java -cp .;%NDDSHOME%\class\nddsjava.jar hello_worldSubscriber -drs 1


    Persistence Service Scenario
    1) Run persistence service (in the same folder that there are 
    persistence_service_configuration.xml): 
        %NDDSHOME%\scripts\rtipersistenceservice.bat -cfgFile persistence_service_configuration.xml 
            -cfgName <persistence_service_database|persistence_service_filesystem>

    2) java -cp .;%NDDSHOME%\class\nddsjava.jar hello_worldSubscriber
    3) java -cp .;%NDDSHOME%\class\nddsjava.jar hello_worldPublisher -sample_count 5 -initial_value 0
    4) java -cp .;%NDDSHOME%\class\nddsjava.jar hello_worldSubscriber 

** On UNIX systems run:
    Durable Writer History scenario
    1) java -cp .:$NDDSHOME/class/nddsjava.jar hello_worldSubscriber
    2) java -cp .:$NDDSHOME/class/nddsjava.jar hello_worldPublisher -sample_count 5 -initial_value 0 -dwh 1 
    3) java -cp .:$NDDSHOME/class/nddsjava.jar hello_worldSubscriber
    4) java -cp .:$NDDSHOME/class/nddsjava.jar hello_worldPublisher -sample_count 5 -initial_value 5 -dwh 1

    Durable Reader State Scenario
    1) java -cp .:$NDDSHOME/class/nddsjava.jar hello_worldSubscriber -drs 1
    2) java -cp .:$NDDSHOME/class/nddsjava.jar hello_worldPublisher -sample_count 5 -initial_value 0 -sleep 60 
    3) stop hello_worldSubscriber
    4) java -cp .:$NDDSHOME/class/nddsjava.jar hello_worldSubscriber
    5) java -cp .:$NDDSHOME/class/nddsjava.jar hello_worldSubscriber -drs 1


    Persistence Service Scenario
    1) Run persistence service (in the same folder that there are 
    persistence_service_configuration.xml): 
        %NDDSHOME%\scripts\rtipersistenceservice -cfgFile persistence_service_configuration.xml 
            -cfgName <persistence_service_database|persistence_service_filesystem>

    2) java -cp .:$NDDSHOME/class/nddsjava.jar hello_worldSubscriber
    3) java -cp .:$NDDSHOME/class/nddsjava.jar hello_worldPublisher -sample_count 5 -initial_value 0
    4) java -cp .:$NDDSHOME/class/nddsjava.jar hello_worldSubscriber 

The applications accepts different arguments:
    hello_worldSubscriber:  
        -domainId <domain ID> (default: 0)
        -sample_count <sample_count> (default: infinite => 0)
        -drs <1|0> Enable/Disable durable reader state (default: 0)

    hello_worldPublisher:
        -domainId <domain ID> (default: 0)
        -sample_count <number of published samples> (default: infinite)
        -initial_value <first sample value> (default: 0)
        -sleep <sleep time in seconds before finishing> (default: 0)
        -dwh <1|0> Enable/Disable durable writer history (default: 0)

