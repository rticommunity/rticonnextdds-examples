# Example Code: Custom Flowcontroller

## Concept
A *FlowController* is the object responsible for shaping the network traffic by
determining when attached asynchronous *DataWriters* are allowed to write data.

You can use one of the built-in *FlowControllers* (and optionally modify their
properties), create a custom *FlowController* by using the *DomainParticipant's*
`create_flowcontroller()` operation, or create a custom *FlowController* by
using the *DomainParticipant*'s `PROPERTY` QosPolicy (DDS Extension).

## Example Description
This example shows how to create a custom flow controllers to manage an
asynchronous publisher.

In the *Asynchronous Publishing* example we showed how asynchronous
publication is useful to aggregate small messages into fewer packets, and
thus achieve lower overhead. In this example we present another use case.

The publisher send a burst of large samples once per second. If many writers
are sending in such bursts (particularly if synchronized), this will increase
collisions, forcing resends in the case of reliable transmission, or causing
data loss for best-effort transmissions. We use asynchronous publication with
a custom flow controller to smooth out these bursts.

No changes are required on the subscriber, but we add a clock to show the
effects of the *FlowController*.
