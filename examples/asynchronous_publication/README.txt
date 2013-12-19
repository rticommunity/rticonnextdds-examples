===========================================
 Example Code -- Asynchronous Publisher
===========================================

Concept
-------
This example shows how to configure a datawriter for asynchronous publication.
This means that instead of sending a sample before returning from write(), 
the data is enqueued and sent according to a flow controller. There are three 
default flowcontrollers available. The first performs no throttling, and allows
data to be send as soon as write() is called. The second, a fixed rate 
flowcontroller, allows data to be sent only once per second. The third allows 
manual control; that is, accumulated data is sent only on calls to 
trigger_flow(). All flowcontrollers allow messages to be coalesced into as few
network packets as possible. 

It is also possible to create custom flow controllers--see example 'Custom 
Flowcontroller'. This example uses the fixed rate flowcontroller. Such a 
flowcontroller might be useful when sending many small samples to minimize 
packet overhead. To illustrate the batching, we write 10 samples per second 
to DDS, which sends them together once per second. 

Example Description
-------------------

In this example, the publisher application is configured to send data 
asynchronously.  This means that when the data is sent, it is stored in the 
DataWriter's queue until the flow controller allows it to be sent.

There are no changes on the DataReader side.
