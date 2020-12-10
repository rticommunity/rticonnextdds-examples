# Network capture: Shared memory and UDP example

This example is a simple Hello World application between a publisher and a
subscriber. It also generates two capture files (one for the publisher and one
for the subscriber) with the RTPS traffic exchanged between them.

For the sake of simplicity, this example:

* Does not modify the transport protocol. The default UDPv4 and Shared Memory
  protocols are used.

* Captures traffic during the whole execution of the application. It starts
  capturing traffic at the beginning (after the creation of the
  DomainParticipant) and stops at the end (after the deletion of the
  DomainParticipant). The exception is the publisher, where the capture is
  paused for a couple of samples.

* Starts network capture globally (i.e., for all participants). That way we
  don't have to specify the particular DomainParticipant as an input to the
  function for starting the capture. Note that this also means that if we add
  new Domain Participants, their traffic will be captured as well.

* Uses the default parameters for the capture. This example does not configure
  the capture (besides choosing the name for the output .pcap files).
  See the example with the Security Plugins for a reference as how to add
  parameters.
