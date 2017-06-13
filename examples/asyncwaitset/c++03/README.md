# Example Code: Asynchronous WaitSet

There are two files in this example:
 - AwsExample_publisher: publisher application
 - AwsExample_subscriber: subscriber application

You will use *rtiddsgen* to generate the additional files required to build
and run the example. The *RTI Connext Core Libraries and Utilities Getting 
Started Guide* describes this process in detail.

## Building C++03 Example
Before compiling or running the example, make sure the environment variable
`NDDSHOME` is set to the directory where your version of *RTI Connext* is
installed.

Run *rtiddsgen* with the `-example` option and the target architecture of your
choice. **Do not use the `-replace` option so that the example files are not modified.** 

For example, for the *i86Linux2.6gcc4.4.5* architecture run:

    rtiddsgen -language C++03 -example i86Linux2.6gcc4.4.5 cft.idl

You will see messages that look like this:

    File <WORKING_DIR>/AwsExample_publisher.cxx already exists and will not be 
    replaced with updated content. If you would like to get a new file with the 
    new content, either remove this file or supply -replace option.
    File <WORKING_DIR>/AwsExample_subscriber.cxx already exists and will not be 
    replaced with updated content. If you would like to get a new file with the 
    new content, either remove this file or supply -replace option.

which only informs you that the subscriber/publisher code has not been replaced
as expected.

Build the example accordingly to the target platform. For example, for the 
*i86Linux2.6gcc4.4.5* architecture run:

    make -f makefile_AwsExample_i86Linux2.6gcc4.4.5


## Running C++03 Example
Run the publisher and subscriber applications in a different terminal each and
from the example directory (this is necessary to ensure the application loads 
the QoS defined in *USER_QOS_PROFILES.xml*).

### Publisher

For i86Linux2.6gcc4.4.5, run:

    ./objs/i86Linux2.6gcc4.4.5/AwsExample_publisher [options]

where the publisher options are:

- `-d, -domainId`: Domain ID. 
    **Default**: 0.
- `-p, -publisherId`: Key value of the samples sent. 
    **Default**: (random).
- `-s, -samples`: Number of samples to send before the application shuts down.
     **Default**: 0 (infinite).
- `-h, -help`: Displays application usage and exits.

### Subscriber

For i86Linux2.6gcc4.4.5, run:

    ./objs/<arch_name>/AwsExample_subscriber

where the subscriber options are:

- `-d, -domainId`: Domain ID. 
    **Default**: 0.
- `-t, -threads`: Number of threads used to process sample reception.
     **Default**: 4.
- `-s, -samples`: Number of received samples before the application shuts down.
     **Default**: 0 (infinite).
- `-h, -help`: Displays application usage and exits.