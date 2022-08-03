# Example Code: Crypto algorithms

## Concept

This example is a secure Hello World application between a publisher and a
subscriber. The publisher and subscriber DomainParticipants will use a different
configuration depending on the scenario passed as the third command-line
argument. These scenarios show some of the possible combinations for the digital
signature, key establishment, and symmetric cipher algorithms. The example also
shows how the `<allowed_security_algorithms>` tag of the Governance Document
affects DomainParticipant matching.

## Example description

You can run a publisher and/or a subscriber with the following scenarios:

-   **scenario0**: Communication between publisher and subscriber should take
    place.
    -   All security algorithms are allowed.
    -   Digital signature algorithm: `ECDSA+P256+SHA256`.
    -   Key establishment algorithm: `ECDHE-CEUM+P256` (detected from the
        Identity Certificate because the default value of the
        `authentication.key_establishment_algorithm` property is `AUTO`).
    -   Symmetric cipher algorithm: `AES256+GCM` (default value of the
        `cryptography.encryption_algorithm` property)

-   **scenario1**: Communication between publisher and subscriber should take
    place.
    -   All security algorithms are allowed.
    -   Digital signature algorithm: `ECDSA+P384+SHA384`.
    -   Key establishment algorithm: `ECDHE-CEUM+P384` (detected from the
        Identity Certificate because the default value of the
        `authentication.key_establishment_algorithm` property is `AUTO`).
    -   Symmetric cipher algorithm: `AES256+GCM` (default value of the
        `cryptography.encryption_algorithm` property)

-   **scenario2**: Communication between publisher and subscriber should take
    place.
    -   The default security algorithms are allowed. These algorithms are the
        ones that are assumed if the DomainParticipants do not transmit the
        information through discovery. You can see using Wireshark that the
        serialized data doesn't contain the PIDs associated to the
        algorithms (compare with the traffic from scenario0).
    -   Digital signature algorithm: `ECDSA+P256+SHA256`.
    -   Key establishment algorithm: `ECDHE-CEUM+P256`.
    -   Symmetric cipher algorithm: `AES256+GCM` (default value of the
        `cryptography.encryption_algorithm` property)
    -   The DomainParticipants from scenario0 and scenario2 can communicate with
        each other. Matching between DomainParticipants from scenario1 and
        scenario2 fail. This is because in scenario2 the system doesn't allow
        the P384 algorithms.

-   **scenario3**: DomainParticipant creation fails because the algorithms are
    not allowed by the Governance Document.

-   **scenario4**: Communication between publisher and subscriber should take
    place. However, because they are different Certificate Authorities,
    validation will succeed with the alternative CA. We will first get an error
    verifying the certificate.
    -   All security algorithms are allowed.
    -   Digital signature algorithm: `ECDSA+P384+SHA384` for the publisher and
        `ECDSA+P256+SHA256` for the subscriber.
    -   Key establishment algorithm: `ECDHE-CEUM+P256` for the publisher and
        `ECDHE-CEUM+P384` for the subscriber.
    -   Symmetric cipher algorithm: `AES128+GCM` for the publisher and for
        `AES256+GCM` the subscriber.

## References

1.  [RTI Security Plugins User Manual](https://community.rti.com/static/documentation/connext-dds/7.0.0/manuals/connext_dds_secure/users_manual/p2_core/cryptography.html#allowed-security-algorithms-domain-rule)
