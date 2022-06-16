# Example Code: Request-Reply

:warning: **Note**: This is available in other languages when you install
**Connext DDS**.

## Concept

Request-reply is a communication pattern offered in addition to
publish-subscribe.  Sometimes an application needs to get a one-time snapshot
of information; for example, to make a query into a database or retrieve
configuration parameters that never change. Other times an application needs to
ask a remote application to perform an action on its behalf; for example, to
invoke a remote procedure call or a service.

See the [User's Manual](https://community.rti.com/static/documentation/connext-dds/6.1.0/doc/manuals/connext_dds_professional/users_manual/index.htm#users_manual/RequestReplyOverview.htm)
for more information.

## Example Description

This example shows how to create two applications:

-   A replier application waits for requests to calculate prime numbers under
    a certain number. When it receives a request, performs the calculation,
    sending replies with intermediate results as they are computed.

-   A requester application sends a request with the number specified in the
    command line, waits for the replies, and prints the prime numbers it
    receives.
