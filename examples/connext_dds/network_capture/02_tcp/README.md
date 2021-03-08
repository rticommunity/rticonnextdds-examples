# Network capture: TCP example

This example is a simple Hello World application between a publisher and a
subscriber. It also generates two capture files (one for the publisher and one
for the subscriber) with the RTPS traffic exchanged between them.

The example can run over TCP LAN (Local Area Network), Symmetric WAN (Wide Area
Network), Asymmetric WAN or TLS (Transport Layer Security). The goal is to
better understand when using the feature for debugging makes sense. Because
network capture saves traffic at the application level, the generated capture
files do not have transport information such as the TCP control messages or the
TLS handshake. But, as an advantage, we have access to the RTPS traffic (while
captures obtained directly with Wireshark will show that the application data
is encrypted).
