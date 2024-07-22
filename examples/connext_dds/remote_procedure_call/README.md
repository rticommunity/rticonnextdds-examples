# Example Code: Remote Procedure Call

## Concept

Remote Procedure Call (RPC) is a specialization of the Request-Reply
communication pattern. It simplifies "client" and "service" applications by
using a service interface with operations that are implemented in the service
and called from the clients.

See the [User's Manual](https://community.rti.com/static/documentation/connext-dds/7.4.0/doc/manuals/connext_dds_professional/users_manual/users_manual/RPC.htm)
for more information.

## Example Description

This example shows how to create a simple Inventory service with operations
to add and remove items and get the current inventory.

-   The service application implements the interface and runs the service.

-   The client application calls the service operations.
