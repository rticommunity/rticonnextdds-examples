# Example Code: Lightweight Security Interoperability

## Concept

This example showcases how to use lightweight security with full security in a
hybrid Connext Secure system. This example requires Connext Secure 7.2.0 and
newer. This example is based on a standard rtiddsgen publisher and subscriber
example code. The code has been modified so that 2 topics are used instead of
one. The publisher and one of the subscribers use full security plugins, whereas
the other subscriber uses lightweight security. The Governance file used
showcases a configuration that is compatible with Lightweight security. However,
one of the topics uses a data_protection_kind ENCRYPT topic rule, which breaks compatibility.
