# Example Code: Real-Time WAN Transport

HelloWorld example showing how to use the RTI Real-Time WAN Transport.

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

-   **NAT (Network Address Translator) traversal:** Ability to communicate
    between DomainParticipants running in a Local Area Network (LAN) that is
    behind a NAT-enabled router, and DomainParticipants on the outside of the
    NAT across a WAN. This functionality is provided in combination with Cloud
    Discovery Service.
-   **IP mobility:** Support for network transitions and changes in IP addresses
    in any of the DomainParticipants participating in the communication.
-   **Security:** Secure communications between DomainParticipants using
    Security Plugins.

Real-Time WAN Transport does not require third-party components, such as STUN
servers, or protocols like SIP to handle session establishment. Using a single
API and security model, you can leverage the extensive capabilities of the
Connext DDS framework and ecosystem, including tools and infrastructure
services, even for real-time connectivity from edge to cloud and back in highly
distributed systems that communicate across wide area networks.

## Definitions

-   **External DomainParticipant:** A DomainParticipant using a Real-Time WAN
    Transport that is publicly reachable at a public address. Being reachable at
    a public IP address does not mean that the DomainParticipant is not behind a
    NAT-enabled router. It is possible that an external DomainParticipant is
    behind a NAT-enabled router if the network administrator configures a static
    NAT mapping between the DomainParticipant private address and a public
    address.

## Example Description

The main goal of this Example is to test connectivity under the following WAN
communication scenarios:

-   **Scenario 1 (default):** Non-secure communication between an internal
    Participant using default UDP port mapping and an External Participant 
    using a single UDP port.
-   **Scenario 2:** Secure communication between an internal Participant using
    default UDP port mapping and an External Participant using a single UDP
    port.
-   **Scenario 3:** Non-secure communication between two internal Participants
    behind Cone-NATs using default UDP port mapping. This scenario requires
    running Cloud Discovery Service.
-   **Scenario 4:** Secure communication between two internal Participants
    behind Cone-NATs using default UDP port mapping. This scenario requires
    running Cloud Discovery Service.
-   **Scenario 5:** Non-secure communication through Routing Service between 
    two internal Participants behind NATs using default UDP port mapping.
    This scenario requires running Routing Service.
-   **Scenario 6:** Secure communication through Routing Service between two
    internal Participants behind NATs using default UDP port mapping.
    This scenario requires running Routing Service.

Scenario 1 and Scenario 2 require one External Participant reachable in a 
public IP address and port as shown in the Figure below. If the External
Participant is behind a NAT, you have to create a static NAT binding between a
private IP address:port (192.168.1.1:1234 in the figure below) and a public
IP address:port (50.10.23.45:2345 in the figure below).

![Internal to External Participant](
   resources/images/InternalToExternal.png?raw=true
   "Internal to External Participant")

Scenarios 3 and 4 require that both Internal Participants are behind a
Cone-NAT and they also require running a Cloud Discovery Service instance
reachable in a public address (60.10.23.45:2345 in the Figure below) to
facilitate the NAT traversal process.

![Internal to Internal Participant](
   resources/images/InternalToInternal.png?raw=true
   "Internal to Internal Participant").

Scenarios 5 and 6 require that both Internal Participants are behind a
NAT and they also require running a Routing Service instance
reachable in a public address (60.10.23.45 in the Figure below) on two port one
for the input participant (16000) and another for the output participant 
(16001).

![Internal to Internal Participant through Routing Service](
   resources/images/InternalToInternalRS.png?raw=true
   "Internal to Internal Participant through Routing Service").

## Prerequisites before running example

1)  Before running the example, make sure the environment variable `NDDSHOME`
    is set to the directory where your version of *RTI Connext* is installed.

2)  Download and install the RTI Security Plugins in order to run scenarios
    2,4 and 6.

3)  Set the load library path to include the Connext DDS libraries and OpenSSL

    On MacOSX you can do that with the command:

    ```sh
    export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${NDDSHOME}/lib/x64Darwin17clang9.0:${NDDSHOME}/third_party/openssl-1.1.1i/x64Darwin17clang9.0/release/lib
    ```

    On Linux you do it with the command:

    ```sh
    export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${NDDSHOME}/lib/x64Linux4gcc7.3.0:${NDDSHOME}/third_party/openssl-1.1.1i/x64Linux4gcc7.3.0/release/lib
    ```

## Prerequisites before running scenario 1 or 2

`real_time_wan_transport_publisher`

The `real_time_wan_transport_publisher` application will create the
External Participant.

1)  Create a static NAT binding between the
    (private IP address:private UDP port) in which the application will receive
    data and a (public IP address:public UDP port).

    For the sake of simplicity, the XML configuration assumes that
    the private UDP port and the public UDP port are 16000. If you want to use
    a different number replace 16000 with the new number in the XML
    configuration file. You can also use different numbers for the private UDP
    port (&lt;host&gt;) and the public UDP port (&lt;public&gt;).

2)  Set the environment variable PUBLIC_ADDRESS to be the
    public IP address in which the application will receive data. Usually this
    is the public IP address of the NAT-enabled router behind which the
    application runs.

`real_time_wan_transport_subscriber`

1)  Set the environment variable `PUBLIC_ADDRESS` to the public IP address in
    which the ``real_time_wan_transport_publisher`` application receives data.

## Prerequisites before running scenario 3 or 4

These scenarios require running Cloud Discovery Service. They also require
that both applications `real_time_wan_transport_subscriber` and
`real_time_wan_transport_publisher` are behind Cone (or Asymmetric) NAT-enabled
router.

1)  Make sure that the publisher and subscriber applications are running behind
    a Cone NAT. There are multiple third-party utilities that you can download
    to find out the NAT type. One example is
    [natat](https://github.com/songjiayang/natat).

2)  Cloud Discovery Service must be reachable at a public IP address. Create
    a static NAT binding between the (private IP address:UDP port)
    in which the Cloud Discovery Service will receive data and a
    (public IP address:UDP port).

    For the sake of simplicity, the XML configuration assumes that
    the UDP port is 7400. If you want to use a different number replace 7400
    with the new number in the XML configuration files `USER_QOS_PROFILES.xml`
    and `CLOUD_DISCOVERY_SERVICE.xml`.

3)  Set the environment variable `PUBLIC_ADDRESS` to the public IP address in
    which the Cloud Discovery services runs. This environment variable will
    have to be set for the publisher and subscriber applications and for RTI
    Cloud Discovery Service.

## Prerequisites before running scenario 5 or 6

These scenarios require running Routing Service. They also require
that both applications `real_time_wan_transport_subscriber` and
`real_time_wan_transport_publisher` are behind Cone (or Asymmetric) NAT-enabled
router.

1)  Create a static NAT binding between the
    (private IP address:private UDP ports) in which the Routing Service will 
    receive/send data and a (public IP address:public UDP ports).

    For the sake of simplicity, the XML configuration assumes that
    the private and public UDP port for the input participat are 16000, and the
    private and public UDP port for the output participat are 16001. If you
    want to use a different number replace 16000/16001 with the new number in
    the XML configuration file. You can also use different numbers for the
    private UDP port (&lt;host&gt;) and the public UDP port (&lt;public&gt;).

2)  Set the environment variable `PUBLIC_ADDRESS` to be the
    public IP address in which the application will receive data. Usually this
    is the public IP address of the NAT-enabled router behind which the
    application runs.

## Running Cloud Discovery Service

To run Cloud Discovery Service in scenarios 3 and 4 you can use the following
command line on Mac OS and Linux:

```sh
${NDDSHOME}/bin/rticlouddiscoveryservice -cfgFile CLOUD_DISCOVERY_SERVICE.xml -cfgName AllDomains
```

Use `AllDomains` for scenario 3 and `AllDomainsSecure` for scenario 4.

## Running Routing Service

To run Routing Service in scenarios 5 and 6 you can usethe following
command line on Mac OS and Linux:

```sh
${NDDSHOME}/bin/rticlouddiscoveryservice -cfgFile RS/ROUTING_SERVICE.xml -cfgName RS
```

Use `RS` for scenario 5 and `RSSecure` for scenario 6.
