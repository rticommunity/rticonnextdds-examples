# Example Code: Real-Time WAN Transport

HelloWorld example showing how to use the RTI Real-Time WAN Transport.

For more information, please refer to the [RTI Connext DDS Core Libraries User's Manual](https://docserver.rti.com/docs/connext-docs/6.1.0.0/doc/manuals/connext_dds_professional/users_manual/index.htm#users_manual/PartRealtimeWAN.htm%3FTocPath%3DPart%25205%253A%2520RTI%2520Real-Time%2520WAN%25C2%25A0Transport%7C_____0).

## Concept

Real-Time WAN Transport is a smart transport that enables secure, scalable, and 
high-performance communication over wide area networks (WANs), including public 
networks. It extends Connext DDS capabilities to WAN environments. 
Real-Time WAN Transport uses UDP as the underlying IP transport-layer protocol 
to better anticipate and adapt to the challenges of diverse network conditions, 
device mobility, and the dynamic nature of WAN system architectures.

Real-Time WAN Transport, in combination with RTI Cloud Discovery Service, 
provides a complete, seamless solution out of the box for WAN connectivity. 
This WAN connectivity solution, including Real-Time WAN Transport and Cloud 
Discovery Service, is available as an optional add-on.

Real-Time WAN Transport provides the following capabilities:

* **NAT (Network Address Translator) traversal:** Ability to communicate between 
DomainParticipants running in a Local Area Network (LAN) that is behind a 
NAT-enabled router, and DomainParticipants on the outside of the NAT across a 
WAN. This functionality is provided in combination with Cloud Discovery Service.
* **IP mobility:** Support for network transitions and changes in IP addresses 
in any of the DomainParticipants participating in the communication
* **Security:** Secure communications between DomainParticipants using Security 
Plugins.

Real-Time WAN Transport does not require third-party components, such as STUN 
servers, or protocols like SIP to handle session establishment. Using a single 
API and security model, you can leverage the extensive capabilities of the 
Connext DDS framework and ecosystem, including tools and infrastructure 
services, even for real-time connectivity from edge to cloud and back in highly 
distributed systems that communicate across wide area networks.

## Definitions

* **External DomainParticipant:** A DomainParticipant using a Real-Time WAN 
Transport that is publicly reachable at a public address. Being reachable at a 
public IP address does not mean that the DomainParticipant is not behind a 
NAT-enabled router. It is possible that an external DomainParticipant is behind 
a NAT-enabled router if the network administrator configures a static NAT 
mapping between the DomainParticipant private address and a public address.

## Example Description

The main goal of this Example is to test connectivity under the following WAN
communication scenarios:

* **Scenario 1 (default):** Non-secure communication between an internal Participant 
using default UDP port mapping and an External Participant using a single UDP 
port.
* **Scenario 2:** Secure communication between an internal Participant 
using default UDP port mapping and an External Participant using a single UDP 
port.
* **Scenario 3:** Non-secure communication between two internal Participants behind
Cone-NATs using default UDP port mapping. This scenario requires running Cloud
Discovery Service.
* **Scenario 4:** Secure communication between two internal Participants behind
Cone-NATs using default UDP port mapping. This scenario requires running Cloud
Discovery Service.

Scenario 1 and Scenario 2 require one External Participant reachable in a public
IP address and port as shown in the Figure below. If the External Participant is
behind a NAT, you have to create a static NAT binding between a
private IP address:port (192.168.1.1:1234 in the figure below) and a public 
IP address:port (50.10.23.45:2345 in the figure below).

![Internal to External Participant](resources/images/InternalToExternal.png?raw=true "Internal to External Participant")

Scenarios 3 and 4 require that both Internal Participants are behind a 
Cone-NAT and they also require running a RTI Cloud Discovery Service instance 
reachable in a public address (60.10.23.45:2345 in the Figure below) to 
facilitate the NAT traversal process. 

![Internal to Internal Participant](resources/images/InternalToInternal.png?raw=true "Internal to Internal Participant").

## Prerequisites before running example

1) Before running the example, make sure the environment variable `NDDSHOME` is 
   set to the directory where your version of *RTI Connext* is installed.

2) Set the load library path to include the Connext DDS libraries.

   On MacOSX you can do that with the command:

   ```sh
   export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${NDDSHOME}/lib/x64Darwin17clang9.0:${NDDSHOME}/third_party/openssl-1.1.1i/x64Darwin17clang9.0/release/lib
   ```

   On Linux you do it with the command:

   ```sh
   export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${NDDSHOME}/lib/x64Linux4gcc7.3.0:${NDDSHOME}/third_party/openssl-1.1.1i/x64Linux4gcc7.3.0/release/lib
   ```

## Prerequisites before running example with Security

1) Download and install the RTI Security Plugins in order to run scenarios
   2 and 4.

## Prerequisites before running scenario 1 or 2

`real_time_wan_transport_subscriber`

The `real_time_wan_transport_subscriber` application will create the
External Participant. 

1) Create a static NAT binding between the (private IP address:private UDP port) 
   in which the application will receive data and a 
   (public IP address:public UDP port).

   For the sake of simplicity, the XML configuration assumes that 
   the private UDP port and the public UDP port are 16000. If you want to use
   a different number replace 16000 with the new number in the XML configuration 
   file. You can also use different numbers for the private UDP port 
   (&lt;host&gt;) and the public UDP port (&lt;public&gt;).

2) Set the environment variable PUBLIC_ADDRESS to be the
   public IP address in which the application will receive data. Uusally this is
   the public IP address of the NAT-enabled router behind which the application 
   runs.

`real_time_wan_transport_publisher`

1) Set the environment variable PUBLIC_ADDRESS to the public IP address in which
   the ``real_time_wan_transport_subscriber`` application receives data.

## Prerequisites before running scenario 3 or 4

These scenarios require running RTI Cloud Discovery Service. They also require
that both applications `real_time_wan_transport_subscriber` and 
`real_time_wan_transport_publisher` are behind Cone (or Assymetric) NAT-enabled 
router.

1) Make sure your applications are running behind a Cone NAT. There are multiple 
   third-party utilities that you can download to find out the NAT type. One 
   example is natat (https://github.com/songjiayang/natat).

2) RTI Cloud Discovery Service must be recahable at a public IP address. Create 
   a static NAT binding between the (private IP address:UDP port) 
   in which the RTI Cloud Discovery Service will receive data and a 
   (public IP address:UDP port).

   For the sake of simplicity, the XML configuration assumes that 
   the UDP port is 7400. If you want to use a different number replace 7400 with 
   the new number in the XML configuration files `USER_QOS_PROFILES.xml` and `CLOUD_DISCOVERY_SERVICE.xml`

## Running the test

To run a specific scenario use the command line option `--scenario <value>`
in both applications.

You can get a full list of command line parameters by running with the
command line option `-h`.

If communication is established you should see the following messages in the
`real_time_wan_transport_subscriber` console output.

```
Received data

   msg: "Hello World 1"
Received data

   msg: "Hello World 2"
```












    