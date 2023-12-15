# Example Code: Lightweight Security with Cloud Discovery Service

## Concept

This example showcases protection of participant announcement messages (Data(p)
packets), including those relayed by Cloud Discovery Service through lightweight
security plugins. This requires Connext Secure 7.2.0 or newer. This example is
based on a standard rtiddsgen publisher and subscriber example code and a basic
Cloud Discovery Service configuration. Initial peers are set up so that
communication can only happen in the presence of Cloud Discovery Service, and in
particular the secure CDS configuration profile.
A Wireshark capture is supplied as part of the example.
