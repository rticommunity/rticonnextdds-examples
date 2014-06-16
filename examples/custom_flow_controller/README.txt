=======================================
 Example Code -- Custom Flowcontroller
=======================================

Concept
-------
In the 'Asynchronous Publishing' we showed how asynchronous publication is 
useful to aggregate small messages into fewer packets, and thus achieve lower 
overhead. In this example we present another use case. 

The asynchronous publication means that instead of sending a sample before 
returning from write(), the data is enqueued and sent according to a flow 
controller. There are three default flow controllers available and you can
create a custom one for your needs.

A FlowController is the object responsible for shaping the network traffic by 
determining when attached asynchronous DataWriters are allowed to write data.

This example shows how to create a custom flow controller to manage a
asynchronous publisher.

Example Description
-------------------
PUBLISHER: 
The publisher send a burst of large samples once per second. If many writers are 
sending in such bursts (particularly if synchronized), this will increase 
collisions, forcing resends in the case of reliable transmission, or causing 
data loss for best-effort transmissions. 

We use asynchronous publication with a custom flow controller to smooth out 
these bursts. 

SUBSCRIBER: 
No changes are required on the subscriber, but we add a clock to show the 
effects of the FlowController. 
