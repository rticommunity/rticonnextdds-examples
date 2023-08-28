# Example Code: Basic Security

## Concept

RTI Security Plugins allow you to address your security requirements in a
granular way. This example assumes that you have a familiarity with RTI Connext.

The main aspects of security that are covered are

-   Authentication - Verifying the identity of the application and/or user that
invokes actions on DDS.
-   Access Control - Provides a way to enforce policy decision on what DDS-related
operations an authenticated entity can perform (i.e. which Domains it can join,
which topics it can publish and subscribe to, etc)
-   Cryptography - Implements (or interfaces with libraries that implement)
cryptographic operations such as encryption, decryption, hashing, digital
signatures, etc
-   Logging - Auditing of all DDS security-related events.

Additionally, The Security Plugins can potentially run over any transport,
including the builtin UDP transport with multicast and TCP transport.
The OMG DDS Security specification addresses the security aspect of the
communication in a one-to-many, friendly, data-centric way, enabling
applications to define different security policies based on the nature of the
shared data. This gives the benefits of no single point of failure, high
performance, and high scalability.

In a DDS Secure system, a Governance Document defines the security requirements
for communication. At the domain level, this file contains a mapping between
Domain IDs and the security policies that DomainParticipants must follow to
interact in that Domain.

As you can see, the rules that compose the Governance Document specify how your
system is protected. All the DomainParticipants in your secure system need to
load the same Governance Document, either by having a copy of it, or by
accessing a single Governance Document from a common location.

In addition to meeting the security requirements specified in the Governance
Document, every DomainParticipant joining a Secure Domain must be associated
with a Permissions Document. This Permissions Document contains a set of grants,
which determine what the local participant is allowed to do in the Domain.

For more information about using RTI Connext with security, follow the
[Getting Started Guide](https://community.rti.com/static/documentation/connext-dds/current/doc/manuals/connext_dds_secure/getting_started_guide/index.html)

## Example Description

This example is available in other programming languages in the
`rti_workspace/<version>/examples/connext_dds/<language>/hello_security`
folder created under your home directory when RTI Connext is installed.

In this example, the publisher application sends a message of "Hello World" with
a number to indicate which sample it is. This number increments by one and by
default will go until the user stops the programs. By default, the messages are
sent using ecdsa but can be set to rsa or ecdsa secp384r1 by providing an
argument at run time.
