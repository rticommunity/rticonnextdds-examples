# Network capture: Secure example

This example is a secure Hello World application between a publisher and a
subscriber. It also generates two capture files (one for the publisher and one
for the subscriber) with the RTPS traffic exchanged between them.

In this example we are capturing traffic globally (for all participants), and
using parameters to configure the capture:

-   The publisher will use the default parameters except for the traffic
    direction. Instead of capturing both the inboud and outbound traffic, only
    the inbound traffic is captured.

-   The subscriber uses the default parameters except for: the capture will
    include the results of decrypting the RTPS message, and it will exclude the
    encrypted contents. Excluding the encrypted contents but including the results
    of the decryption is done to reduce the filesize.
