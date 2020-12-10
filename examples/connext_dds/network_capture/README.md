# Example Code: Network Capture

## Concept

*Connext DDS* allows you to capture network traffic that one or more
*DomainParticipants* send or receive. This feature can be used to analyze and
debug communication problems between your DDS applications. When network
capture is enabled, each *DomainParticipant* will generate a pcap-based file
that can then be opened by a packet analyzer like Wireshark, provided the right
dissectors are installed.

To some extent, network capture can be used as an alternative to existing
pcap-based network capture software (such as Wireshark). This will be the case
when you are only interested in analyzing the traffic a *DomainParticipant*
sends/receives. In this scenario, network capture will actually have some
advantages over more general pcap-based network capture applications: RTI's
network capture includes additional information, such as security-related data;
it also removes information that is not needed, such as user data, when you
want to reduce the capture size. That said, RTI's network capture is not a
replacement for other pcap-based network capture applications: it only captures
the traffic exchanged by the DomainParticipants, but it does not capture any
other traffic exchanged through the system network interfaces.

To capture network traffic, it must be enabled before creating any
*DomainParticipant*. Similarly, it must be disabled before deleting all
*DomainParticipants*. In between these calls, you may start, stop, pause, or
resume capturing traffic for one or all *DomainParticipants*.

## Example description

These examples illustrate how to use network capture through some common
scenarios. Each example is independent of the rest, so you can jump directly to
a particular one. However, they are ordered in increasing complexity, so in
order to better learn about the feature, it is recommended to start at the
beginning. The first example is a hello world with the default transport
protocols (UDPv4 and Shared Memory). The second example is the same application
but running over TCP LAN, WAN or TLS. Then we have an example running with the
Security Plugins enabled. More advanced scenarios are considered in the
remaining examples.

Running each example generates a set of .pcap capture files that can then be
opened with Wireshark (if the right dissector is installed). Therefore, even if
*Connext DDS* can generate the capture files, we still require Wireshark as a
tool for analyzing the output files.

For learning purposes, it is also recommended to capture the traffic from the
examples with Wireshark. That way, you can better understand the differences
between the resulting capture files (and therefore, when it makes more sense to
use one tool over the other for capturing traffic).

## References

1. [RTI Core Libraries User Manual](https://community.rti.com/static/documentation/connext-dds/6.1.0/doc/manuals/connext_dds/html_files/RTI_ConnextDDS_CoreLibraries_UsersManual/index.htm#users_manual/NetworkCapture.htm)

2. [RTI Modern C++ API Reference](https://community.rti.com/static/documentation/connext-dds/current/doc/api/connext_dds/api_cpp2/group__NDDSNetworkCaptureModule.html)

## TODO

- Check links once Connext 6.1.0 is released.
- Complete readme files
- Test the c# example in Windows with the new build system
