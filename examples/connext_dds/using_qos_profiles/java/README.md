# Example Code: Using QoS Profiles

## Building Java Example

Before compiling or running the example, make sure the environment variable
`NDDSHOME` is set to the directory where your version of *RTI Connext* is
installed.

Run *rtiddsgen* with the `-example` option and the target architecture of your
choice (e.g., *i86Win32VS2010* or *i86Linux2.6gcc4.4.5*). The *RTI Connext Core
Libraries and Utilities Getting Started Guide* describes this process in detail.
Follow the same procedure to generate the code and build the examples. **Do not
use the `-replace` option.** Assuming you want to generate an example for
*i86Win32VS2010* run:

```sh
rtiddsgen -language Java -example i86Win32VS2010 profiles.idl
```

You will see messages that look like this:

```plaintext
File C:\local\using_qos_profiles\java\profilesSubscriber.java already exists
and will not be replaced with updated content. If you would like to get a new
file with the new content, either remove this file or supply -replace option.
File C:\local\using_qos_profiles\java\profilesPublisher.java already exists and
will not be replaced with updated content. If you would like to get a new file
with the new content, either remove this file or supply -replace option.
```

This is normal and is only informing you that the subscriber/publisher code has
not been replaced, which is fine since all the source files for the example are
already provided.

Before compiling in Java, make sure that the desired version of the *javac*
compiler is in your `PATH` environment variable.

On *Windows* systems run:

```sh
javac -classpath .;%NDDSHOME%\lib\java\nddsjava.jar profiles.java profilesSeq.java profilesTypeSupport.java profilesTypeCode.java profilesDataReader.java profilesDataWriter.java profilesSubscriber.java profilesPublisher.java
```

On *UNIX* systems run:

```sh
javac -classpath .:$NDDSHOME/lib/java/nddsjava.jar profiles.java profilesSeq.java profilesTypeSupport.java profilesTypeCode.java profilesDataReader.java profilesDataWriter.java profilesSubscriber.java waitsetsPublisher.java
```

After building the example, put *USER_QOS_PROFILES.xml* and
*my_custom_qos_profiles.xml* in the directory you will run your application
from.

## Running Java Example

In two separate command prompt windows for the publisher and subscriber. Run the
following commands from the example directory (this is necessary to ensure the
application loads the QoS defined in *USER_QOS_PROFILES.xml* and
*my_custom_qos_profiles.xml*):

On *Windows* systems run:

```sh
java -cp .;%NDDSHOME%\lib\java\nddsjava.jar profilesPublisher  <domain_id>
    <samples_to_send>
java -cp .;%NDDSHOME%\lib\java\nddsjava.jar profilesSubscriber <domain_id>
    <sleep_periods>
```

On *UNIX* systems run:

```sh
java -cp .:$NDDSHOME/lib/java/nddsjava.jar profilesPublisher  <domain_id>
    <samples_to_send>
java -cp .:$NDDSHOME/lib/java/nddsjava.jar profilesSubscriber <domain_id>
    <sleep_periods>
```

The applications accept two arguments:

1.  The `<domain_id>`. Both applications must use the same domain ID in order to
    communicate. The default is 0.

2.  How long the examples should run, measured in samples for the publisher and
    sleep periods for the subscriber. A value of '0' instructs the application
    to run forever; this is the default.

### Publisher Output

```plaintext
Writing profiles, count 0
Writing profile_name = volatile_profile x = 0
Writing profile_name = transient_local_profile x = 0
Writing profiles, count 1
Writing profile_name = volatile_profile x = 1
Writing profile_name = transient_local_profile x = 1
Writing profiles, count 2
Writing profile_name = volatile_profile x = 2
Writing profile_name = transient_local_profile x = 2
Writing profiles, count 3
Writing profile_name = volatile_profile x = 3
Writing profile_name = transient_local_profile x = 3
```

### Subscriber Output

Notice that the volatile *DataReader* receives both transient local and volatile
data, because the QoS is request/offered, and volatile QoS matches with the
higher transient local level of service.

```plaintext
profiles subscriber sleeping for 4 sec...
TransientLocalProfile listener received

profiles subscriber sleeping for 4 sec...
======================================

Received:
    profile_name: transient_local_profile
    x: 0

profiles subscriber sleeping for 4 sec...
========================================
VolatileProfile listener received

======================================

Received:
    profile_name: volatile_profile
    x: 1

========================================
VolatileProfile listener received

======================================

Received:
    profile_name: transient_local_profile
    x: 1
```
