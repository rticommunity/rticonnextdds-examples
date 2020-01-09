# Routing Service with Security example

## Concept
This example is an extension to the security example in the 
<rti_workspace folder>/examples/connext_dds/c++/hello_security. 
Routing Service supports DDS security, and may be configured to bridge topic data between:
- 2 unsecured domains (no DDS security) or 
- 1 secured domain and 1 unsecured domain or
- 2 secured domains

## Example description
This example demonstrates Routing Service bridging 1 secured domain and 1 unsecured domain.
The Hello_security publisher resides in Domain 2, a secured domain, 
and the Hello_security subscriber resides in Domain 1, an unsecured domain.

-----------------------------          -----------------------------
|   Domain 1 (unsecured)    |          |   Domain 2 (secured)      |
|                           |          |                           |
|  |----------------|    |----------------|    |----------------|  |
|  | Hello_security |<---| Routing Service|<---| Hello_security |  |
|  |   Subscriber   |    |                |    |    Publisher   |  |
|  |----------------|    |----------------|    |----------------|  |
|                           |          |                           |
-----------------------------          -----------------------------

In the secured domain, security artifacts need to be created for all communicating participants,
i.e. the Hello_security participant and the Routing Service participant.
The security artifacts in this example was generated using OpenSSL 1.0.2o. 
The certificates of this example are signed based on the ECDSA algorithm (default in the hello_security example).
Refer to the RTI Security Plug-ins User's Manual at https://community.rti.com/documentation for the artifacts required, 
and the steps to generate the artifacts.

In this example, data is only routed from Domain 2 to Domain 1.


## Before running the example
This example modifies the QoS profile file in <rti_workspace folder>/examples/connext_dds/c++/hello_security.
To preserve the original file, we will make a new copy of this folder before building this example.

1. Duplicate a copy of the folder <rti_workspace folder>/examples/connext_dds/c++/hello_security to
   <rti_workspace folder>/examples/connext_dds/c++/hello_security_01.

2. Untar the example into folder hello_security_01.


## How to run the example
1. Follow the instructions to compile the Hello_security Example Application.
2. To start the publisher on Domain 2 and the subscriber on Domain 1, type the following in two different command shells, either
on the same machine or on different machines:

> objs\<architecture>\HelloWorld_subscriber 1
> objs\<architecture>\HelloWorld_publisher 2

3. Start the Routing Service on a different command shell, either on the same machine or on different machines:

> rtiroutingservice -cfgName security_example