# Example: Routing Service Constrained WAN

## Description

These files are used to configure routing service for use in a constrained WAN
environment. For a full description of how to use these files see the example
located on the [RTI community website](
https://community.rti.com/examples/Routing-Service-Constrained-WAN-Example)

## Concept

This example shows how to utilize routing service's features such as time
based filters, batching, propagation of content filters, and transformations
to limit the amount of bandwidth DDS samples require. This can be useful to
use cases such as autonomous vehicles that require a high sample rate of data
on the vehicle, while also utilizing a WAN link to a backend server that
has a much more limited network link. These filters can be applied without
any changes to the original publishing applications.

This example also will walk you through using admin console and rtirssh
to control the running routing service process via the remote administration
interface. Existing routes can have new filters applied to them by updating
their routes using the 300ms.xml and 1000ms.xml files.

## Instructions

1.  Start two copies of the shapesdemo, a publisher on domain 0 and a
subscriber on domain 1.

2.  Start routing service using this configuration, from the command
line execute:  
`rtiroutingservice -cfgFile <path to xml>/localhost.xml -cfgName example`  
You should now see shapes routed to your subscriber on domain 1

3.  From the command line run: `rtirssh -domainId 0 -timeout 3`

4.  Update the square route to include a time based filter  
From the rtirssh shell run:
`update example Route::Session::Square <path to xml>/300ms.xml`  
Notice how the square is now only received on domain 1 every 300ms

5.  Enable batching by first disabling the triangle's route, and then
enabling the TriangleBatch route.  
This can be done by launching admin console, selecting the routing service
instance in the "physical view", and then right clicking on the Triangle
route to disable it.  
Repeat this process on the TriangleBatch route to enable it.  
Notice how the triangle is now updating in batches of 6 samples,
compared to the other shapes that are updating one sample at a time.

6.  Combine batching and time filters together  
From the rtirssh shell run:
`update example Route::Session::TriangleBatch <path to xml>\300ms.xml`  
Notice how the triangle is still batching 6 samples up at a time, but
it is also time filtering one sample every 300ms.

7.  Combine multiple topics into a single topic  
Within admin console, disable the TriangleBatch route, and enable the
TriangleToCircle route (similar to step 5).  
Notice how the triangle is now remapped to circles.
