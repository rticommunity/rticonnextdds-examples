===========================================================
Example Code - High Priority First Flow Controller Example
===========================================================

Concept
-------
A flow controller can be used to shape the output traffic.
There are many types of flow controllers, including a
high priority one (DDS_HPF_FLOW_CONTROLLER_SCHED_POLICY) that sends 
out higher priority samples first. Priority can be assigned on a
per sample basis, per channel basis (if using multiple channels), or
per writer basis. 
 
Using a flow-controller requires the use of an asynchronous publishing
(ASYNCHRONOUS_PUBLISH_MODE_QOS). With asynchronous publishing, a DataWriter's 
write*() call simply queues up the messages. A background thread delivers 
the actual samples based on the scheduling policy specified by the
flow controller.

Large data is defined as samples larger than the message_size_max of the installed 
transports. Sending large data requires a sample to be fragmented into smaller 
messages, and reassembled at the other end. Sending large data RELIABLY 
requires the use of an asynchronous publisher.

The default QoS configuration must be modified to use a high priority
flow controller and asynchronous publishing. Further optimizations can be made
for large data. 


Example Description
-------------------
This Example shows how to configure a High Priority First Flow Controller, and 
demonstrates it for Large Data being sent reliably. 

The following changes are made to the default QoS configuration file auto-generated 
by rtiddsgen.
(NOTE: These changes are documented as XML comments in USER_QOS_PROFILES.xml)

1. Increase the transport sizes
   - Send and receive buffers
   - Gather-send buffers to maximize message coalescing/batching for a destination
   
   May require the OS to be tuned to match these settings.
   
   Stock Ubuntu Linux 12.04 appears to be ok out of the box.
 
2. Increase the participant resources to increase the maximum message size

3. Install a High Priority First Flow Controller on the participant.

   The example also shows how to tune the flow controller for a certain rate. 
   This can be skipped/customized as needed.

4. Configure data writer to use the High Priority First flow controller installed 
   with the participant. 
   
   Assigns a priority of 10 to the DataWriter (for illustration).

   4.1 Assuming LARGE DATA will be sent, optimize the RESOURCE_LIMITS
       and the PROTOCOL. Those settings can be skipped if large data is not 
       being used.

5. Assuming LARGE DATA will be sent, optimize the settings.
   These settings can be skipped if large data is not being used.
   

The example uses a modified HelloWorld example to show the High Priority First 
Flow Controller. The stock examples is modified as follows:

1. $NDDSHOME/example/CPP/Hello_idl/src/HelloWorld.idl is copied to hpf.idl, and
   modified to increase the
         HPF_MAX_PAYLOAD_SIZE = 8192*16; // 131MB
   so that we can easily test the the flow controller with asynchronous large 
   data.

2. A HelloWorld example is generated in C++
     rtiddsgen -example x64Darwin10gcc4.2.1 hpf.idl 

3. The generated hpf_publisher.cxx is modified top add a line tagged with <<HPF>>
    instance->payload.ensure_length(HPF_MAX_PAYLOAD_SIZE, HPF_MAX_PAYLOAD_SIZE); //<<HPF>>

4. The USER_QOS_PROFILES.xml is modified to configure a High Priority First Flow controller.
 

References
----------
1. RTI_CoreLibrariesAndUtilities_UsersManual, Section 6.6
   FlowControllers (DDS Extension)

2. RTI_CoreLibrariesAndUtilities_UsersManual, Section 15.6.2 
   Setting the Maximum Gather-Send Buffer Count for UDPv4 and UDPv6

3. Tune Your OS for Performance: 
   http://community.rti.com/best-practices/tune-your-os-performance

