# Example Code: Request-Reply DynamicData Application

## Concept

*Dynamic Data* API allows user to create topic types in a programmatic manner
without defining an IDL at compile time.

*Dynamic Data* provides an easy way to access the members of a data sample. Such
members can be primitive data types or complex data types such as structs,
arrays or sequences.

As applications become more complex, it often becomes necessary to use other
communication patterns in addition to publish-subscribe.

Connext DDS includes support for the *Request-Reply* communication pattern,
which has two roles:

-   The requester (service consumer or client) sends a request message and waits
for a reply message.
-   The replier (service provider) receives the request message and responds with
a reply message.

## Example description

This example shows how to establish a Request-Reply communication pattern using
the Request-Reply API based on Dynamic Data types.

Within this example the Requester application requests a list of prime numbers
from `1` to `n`. Then the Replier application computes the prime numbers and
replies with the given sequence of prime numbers to the Requester.

For this example we dinamicaly define two data types, one for the request
samples and the other for the reply samples. The request data type is the
following:

```cpp
struct PrimeNumberRequest {
    long n;
    long primes_per_reply;
}
```

And then the reply data types:

```cpp
const long PRIME_SEQUENCE_MAX_LENGTH = 1024;
enum PrimeNumberCalculationStatus {
    REPLY_IN_PROGRESS,
    REPLY_COMPLETED,
    REPLY_ERROR
};

struct PrimeNumberReply {
    sequence<long, PRIME_SEQUENCE_MAX_LENGTH> primes;
    PrimeNumberCalculationStatus status;
};
```

All the Reply/Request communication is done using the previously mentioned
Dynamic Data data types that are defined in "_Primes.hpp_".
