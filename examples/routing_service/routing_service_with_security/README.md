# Routing Service with Security example

## Concept
Routing Service supports DDS security, and may be configured to bridge topic data between:
- 2 unsecured domains (no DDS security) or 
- 1 secured domain and 1 unsecured domain or
- 2 secured domains

## Example description
This example demonstrates Routing Service bridging 
1 secured domain and 1 unsecured domain using the RTI DDS Ping utility.
The rtiddsping publisher resides in Domain 0, an unsecured domain and 
the rtiddsping subscriber resides in Domain 1, a secured domain.

---------------------------          ---------------------------
|   Domain 0 (unsecured)  |          |   Domain 1 (secured)    |
|                         |          |                         |
|  |--------------|    |----------------|    |--------------|  |
|  |  rtiddsping  |--->| Routing Service|--->|  rtiddsping  |  |
|  |   Publisher  |    |                |    |  Subscriber  |  |
|  |--------------|    |----------------|    |--------------|  |
|                         |          |                         |
---------------------------          ---------------------------

In the secured domain, security artifacts need to be created for all communicating participants,
i.e. the rtiddsping participant and the Routing Service participant.
The security artifacts in this example are located in rticonnextdds-examples/resources/certAuthority. 
The certificates for this example are signed based on the DSA algorithm.

In this example, data is only routed from Domain 0 to Domain 1.


## Before running the example
Linux-based
-----------
If using dynamic libraries, your LD_LIBRARY_PATH must include
$NDDSHOME/lib/<architecture> and $RTI_OPENSSLHOME/<architecture>/<release or debug>/lib
(location of libcrypto.so and libssl.so).


Windows
-------
If using dynamic libraries, your PATH must include
%NDDSHOME%\lib\<architecture> and %RTI_OPENSSLHOME%\<architecture>\<release or debug>\bin
(location of libeay32.dll and ssleay32.dll).  The provided projects use the dynamic
openssl libraries.  To use the static versions change the names of libeay32 ssleay32
to libeay32z and ssleay32z respectively.and the path to
%RTI_OPENSSLHOME%\<architecture>\static_[release|debug]\lib.


## How to run the example
1. Start a command shell at the example directory path (administrator mode for Windows), execute the batch file signFiles.bat to sign the governance file and permission files using the CA certificate and key.

2. To start the publisher on Domain 0 and the subscriber on Domain 1, type the following in two different command shells, either
on the same machine or on different machines:

> rtiddsping -publisher -domainId 0
> rtiddsping -subscriber -domainId 1 -qosFile securedPeerQosProfile.xml -qosProfile SecurityExampleProfiles::SecuredPeer


3. Start the Routing Service on a different command shell, either on the same machine or on different machines:

> rtiroutingservice -cfgFile routingServiceProfile.xml -cfgName security_example
