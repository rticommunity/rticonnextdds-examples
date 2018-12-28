# Example code: C++11 lambda content filter

## Building C++11 Example
Run *rtiddsgen* with the `-example` option and the target architecture of your
choice (e.g., *i86Win32VS2013* or *i86Linux3gcc4.8.2*). The *RTI Connext Core
Libraries and Utilities Getting Started Guide* describes this process in detail.
Follow the same procedure to generate the code and build the examples. **Do not
use the `-replace` option.** Assuming you want to generate an example for
*i86Win32VS2013* run:
```
rtiddsgen -language C++11 -example i86Win32VS2013 LambdaFilterExample.idl
```

You will see messages that look like this:
```
File C:\local\lambda_content_filter\c++11\LambdaFilterExample_subscriber.cxx
already exists and will not be replaced with updated content. If you would like
to get a new file with the new content, either remove this file or supply
-replace option.
File C:\local\lambda_content_filter\c++11\LambdaFilterExample_publisher.cxx
already exists and will not be replaced with updated content. If you would like
to get a new file with the new content, either remove this file or supply
-replace option.
```

This is normal and is only informing you that the subscriber/publisher code has
not been replaced, which is fine since all the source files for the example are
already provided.

## Running C++11 Example
### Publisher/Subscriber in command prompt
In two separate command prompt windows for the publisher and subscriber.

On *Windows* systems run:
```
objs\<arch_name>\LambdaFilterExample_publisher.exe <domain_id> <sample #>
objs\<arch_name>\LambdaFilterExample_subscriber.exe <domain_id> <sample #>
```

On *UNIX* systems run:
```
./objs/<arch_name>/LambdaFilterExample_publisher  <domain_id> <sample #>
./objs/<arch_name>/LambdaFilterExample_subscriber <domain_id> <sample #>
```

The applications accept up to two arguments:

1. The `<domain_id>`. Both applications must use the same domain ID in order
to communicate. The default is 0.
2. How long the examples should run, measured in samples. A value of '0'
instructs the application to run forever; this is the default.
