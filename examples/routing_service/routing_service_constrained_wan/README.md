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
andy changes to the original publishing applications.

This example also will walk you through out to use admin console and rtirssh 
to control the running routing service process via the remote administration 
interface. Existing routes can have new filters applied to them by updating 
their routes using the 300ms.xml and 1000ms.xml files. 

To run routing service using this configuration, from the command line 
execute:  
`rtiroutingservice -cfgFile <path to xml>/localhost.xml -cfgName example`

Routes can be enabled and disabled via the admin console GUI or via:  
`disable example Route::Session::Triangle`  
`enable example Route::Session::TriangleBatch`

Routes can be updated via:  
`update example Route::Session::TriangleBatch <path to xml>\300ms.xml`
