# SHMEM <--> UDP Gateway

This repository contains the necessary files to run the RTI Routing Service
example from the "How does Connext help you meet the constraints of the
hospital's IT department?" blogpost. Using RTI DDS Ping as a pub/sub
application and Routing Service you will be able to create a gateway between
a Shared Memory domain and a UDP domain that uses only 3 ports. There is also
a second configuration option using RTI Cloud Discovery Service to use Unicast.

## Environment variables

For your convenience, there are 2 scripts to set up environment variables:

- Linux: _variables.sh_
- Windows: _variables.bat_

These are the variables they contain, which you should modify according to your
system:

- **NDDSHOME**: path to the installation of RTI Connext Professional.
- **NDDS_QOS_PROFILES**: path to the _qos.xml_ file containing QoS profiles.
- **SHMEM_DOMAIN**: the domain for the applications using SHMEM.
- **UDP_DOMAIN**: the domain for the applications using UDP (mainly the DomainParticipant of RTI Routing Service).
- **APPS**: total number of DomainParticipants on the localhost. By default, Connext will try to reach out to the first 5 created applications on SHMEM, therefore, we need to increase that number if there are more than 5 applications.
- **CDS_IP_ADDRESS**: the IP address of the host that contains RTI Cloud Discovery Service.
- **CDS_PORT**: the UDP port that CDS will use.

On Linux, you can use the environment variables by sourcing the file:

```bash
source variables.sh
```

On Windows, simply run it:

```bash
> variables.bat
```

For convenience, the rest of the README will use the Linux variable sign ($)
instead of the Windows variable signs (%%).

## Multicast example

You will need 3 terminals to run this example.

1. On terminal 1, source the variables script and run an RTI DDS Ping publisher
on SHMEM acting as a local publisher:

    ```bash
    source variables.sh
    $NDDSHOME/bin/rtiddsping -pub -domain $SHMEM_DOMAIN -qosProfile "example_library::shmem_profile"
    ```

2. On terminal 2, source the variables script and run an RTI DDS Ping
subscriber on UDP acting as a remote subscriber:

    ```bash
    source variables.sh
    $NDDSHOME/bin/rtiddsping -sub -domain $UDP_DOMAIN -qosProfile "example_library::multicast"
    ```

3. At this point, there should be no communication between both applications.
They are on different domains and they're using different transports.
On terminal 3, on the same host as the SHMEM application,
source the variables script and start Routing Service:

    ```bash
    source variables.sh
    $NDDSHOME/bin/rtiroutingservice -cfgFile RS_config_multicast.xml -cfgName gateway_SHMEM_and_UDP
    ```
4. The subscriber should now be receiving data. For instance:

    ```bash
    ...
    Current alive publisher tally is: 1
    rtiddsping, issue received: 0000002
    rtiddsping, issue received: 0000003
    rtiddsping, issue received: 0000004
    rtiddsping, issue received: 0000005
    ...
    ```

5. (Optional) Feel free to explore the QoS and RS config files. The relevant
QoS profiles for this example are _shmem_profile_ and _multicast_. The RS file
contains a _domain_route_ with 2 DPs. 1 for UDP and another one for SHMEM
(configured through the DP QoS). It also contains 2 _auto_topic_route_ tags
that allow the traffic to flow in the SHMEM --> UDP and SHMEM <-- UDP
directions. In a real scenario, there would most likely be more topic routes,
because different topics will require different DW / DR QoS policies.

6. (Optional) You can run Wireshark and capture data to verify that the
traffic only goes to the 3 different ports that Routing Service opens:
Multicast discovery, Unicast discovery and Unicast user-data. Which ports are
actually in use will depend on the domain ID you use for UDP and whether you
started the RTI DDS Ping application on the same machine as Routing Service or
not. Remember you can check the ports in use on this [spreadsheet](https://d2vkrkwbbxbylk.cloudfront.net/sites/default/files/knowledge_base/Port%20Assign4.2e.xls).

6. You can now shutdown the 3 applications with Ctrl+C.

## Multicast-less example (CDS)

You will need 3 terminals to run this example.

1. On terminal 1, source the variables script and run an RTI DDS Ping publisher
on SHMEM acting as a local publisher:

    ```bash
    source variables.sh
    $NDDSHOME/bin/rtiddsping -pub -domain $SHMEM_DOMAIN -qosProfile "example_library::shmem_profile"
    ```

2. Install the CDS package. For instance: _rti_cloud_discovery_service-7.2.0-host-x64Linux.rtipkg_

3. On terminal 2, source the variables script and start CDS:

    ```bash
    $NDDSHOME/bin/rticlouddiscoveryservice -cfgFile CDS_config.xml -cfgName cds_all_domains_udpv4
    ```

4. On terminal 3, source the variables script and run an RTI DDS Ping
subscriber on UDP acting as a remote subscriber:

    ```bash
    source variables.sh
    $NDDSHOME/bin/rtiddsping -sub -domain $UDP_DOMAIN -qosProfile "example_library::no_multicast"
    ```

5. At this point, there should be no communication between both applications.
They are on different domains and they're using different transports. On
terminal 4, on the same host as the SHMEM application, source the variables
script and start Routing Service:

    ```bash
    source variables.sh
    $NDDSHOME/bin/rtiroutingservice -cfgFile RS_config_with_CDS.xml -cfgName gateway_SHMEM_and_UDP
    ```

6. The subscriber should now be receiving data. For instance:

    ```bash
    ...
    Current alive publisher tally is: 1
    rtiddsping, issue received: 0000002
    rtiddsping, issue received: 0000003
    rtiddsping, issue received: 0000004
    rtiddsping, issue received: 0000005
    ...
    ```


7. (Optional) Feel free to explore the QoS and RS config files. The relevant
QoS profiles for this example are _shmem_profile_ and _no_multicast_. The RS
file contains a _domain_route_ with 2 DPs. 1 for UDP and another one for SHMEM
(configured through the DP QoS). It also contains 2 _auto_topic_route_ tags
that allow the traffic to flow in the SHMEM --> UDP and SHMEM <-- UDP
directions. In a real scenario, there would most likely be more topic routes,
because different topics will require different DW / DR QoS policies.

8. (Optional) You can run Wireshark and capture data to verify that the
traffic only goes to the 2 different ports that Routing Service opens: Unicast
discovery and Unicast user-data. Which ports are actually in use will depend
on the domain ID you use for UDP and whether you started the RTI DDS Ping
application on the same machine as Routing Service or not. Remember you can
check the ports in use on this [spreadsheet](https://d2vkrkwbbxbylk.cloudfront.net/sites/default/files/knowledge_base/Port%20Assign4.2e.xls).

8. You can now shutdown the 4 applications with Ctrl+C.
