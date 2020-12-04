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

The examples illustrate how to use network capture through some common
scenarios.

## References

1. [RTI Core Libraries User Manual](add_link_here)

2. [RTI C API Reference](add_link_here)

## TODO

- Add links
- Complete readme files
- Test the c# example in Windows with the new build system
