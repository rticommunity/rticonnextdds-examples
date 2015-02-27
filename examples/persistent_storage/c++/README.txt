===========================================
 Example Code -- Persistent Storage
===========================================

Building C++ Example
====================

Before compiling or running the example, make sure the environment variable 
NDDSHOME is set to the directory where your version of RTI Connext is installed.

Run rtiddsgen with the -example option and the target architecture of your 
choice (e.g., i86Win32VS2005 or i86Linux2.6gcc4.4.3). The RTI Connext Core 
Libraries and Utilities Getting Started Guide describes this process in detail. 
Follow the same procedure to generate the code and build the examples. Do not 
use the -replace option.

On Windows systems (assuming you want to generate an example for 
i86Win32VS2005) run:

rtiddsgen -language C++ -example i86Win32VS2005 hello_world.idl

On UNIX systems (assuming you want to generate an example for 
i86Linux2.6gcc4.4.3) run:

rtiddsgen -language C++ -example i86Linux2.6gcc4.4.3 hello_world.idl

You will see messages that look like this:

File C:\local\persistence_storage\c++\hello_world_subscriber.cxx already exists
and will not be replaced with updated content. If you would like to get a new 
file with the new content, either remove this file or supply -replace option.
File C:\local\persistence_storage\c++\hello_world_publisher.cxx already exists
and will not be replaced with updated content. If you would like to get a new 
file with the new content, either remove this file or supply -replace option.
File C:\local\persistence_storage\c++\USER_QOS_PRIFLES.xml already exists and
will not be replaced with updated content. If you would like to get a new file 
with the new content, either remove this file or supply -replace option.

This is normal and is only informing you that the subscriber/publisher code has 
not been replaced, which is fine since all the source files for the example are 
already provided.

Running C++ Example
===================
Use two separate command prompts for publisher and subscriber. Run the following
commands from the example directory (this is necessary to ensure the application
loads the QoS defined in USER_QOS_PROFILES.xml):

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
    1) objs\<arch_name>\hello_world_subscriber.exe
    2) objs\<arch_name>\hello_world_publisher.exe -sample_count 5 -initial_value 0 -dwh 1 
    3) objs\<arch_name>\hello_world_subscriber.exe
    4) objs\<arch_name>\hello_world_publisher.exe -sample_count 5 -initial_value 5 -dwh 1

    Durable Reader State Scenario
    1) objs\<arch_name>\hello_world_subscriber.exe -drs 1
    2) objs\<arch_name>\hello_world_publisher.exe -sample_count 5 -initial_value 0 -sleep 60 
    3) stop hello_world_subscriber
    4) objs\<arch_name>\hello_world_subscriber.exe
    5) objs\<arch_name>\hello_world_subscriber.exe -drs 1


    Persistence Service Scenario
    1) Run persistence service (in the same folder that there are 
    persistence_service_configuration.xml): 
        %NDDSHOME%\bin\rtipersistenceservice.bat -cfgFile persistence_service_configuration.xml 
            -cfgName <persistence_service_database|persistence_service_filesystem>

    2) objs\<arch_name>\hello_world_subscriber.exe
    3) objs\<arch_name>\hello_world_publisher.exe -sample_count 5 -initial_value 0
    4) objs\<arch_name>\hello_world_subscriber.exe 

** On UNIX systems run:
    Durable Writer History scenario
    1) ./objs/<arch_name>/hello_world_subscriber
    2) ./objs/<arch_name>/hello_world_publisher -sample_count 5 -initial_value 0 -dwh 1 
    3) ./objs/<arch_name>/hello_world_subscriber
    4) ./objs/<arch_name>/hello_world_publisher -sample_count 5 -initial_value 5 -dwh 1

    Durable Reader State Scenario
    1) ./objs/<arch_name>/hello_world_subscriber -drs 1
    2) ./objs/<arch_name>/hello_world_publisher -sample_count 5 -initial_value 0 -sleep 60 
    3) stop hello_world_subscriber
    4) ./objs/<arch_name>/hello_world_subscriber
    5) ./objs/<arch_name>/hello_world_subscriber -drs 1


    Persistence Service Scenario
    1) Run persistence service (in the same folder that there are 
    persistence_service_configuration.xml): 
        %NDDSHOME%\bin\rtipersistenceservice -cfgFile persistence_service_configuration.xml 
            -cfgName <persistence_service_database|persistence_service_filesystem>

    2) ./objs/<arch_name>/hello_world_subscriber
    3) ./objs/<arch_name>/hello_world_publisher -sample_count 5 -initial_value 0
    4) ./objs/<arch_name>/hello_world_subscriber 

The applications accepts different arguments:
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

