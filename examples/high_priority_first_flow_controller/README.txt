===========================================================
Example Code - High Priority First Flow Controller Example
===========================================================

Concept
-------
				
Example Description
-------------------
This Example shows how to configure a High Priority First Flow Controller modifying
the following items -- all the key changes (from the generated example) are documented 
as XML comments in USER_QOS_PROFILES.xml. 

1. Increase the transport sizes
   - Send and receive buffers
   - Gather-send buffers to maximize message coalescing/batching for a destination
   
   May require the OS to be tuned to match these settings.
   
   Stock Ubuntu Linux 12.04 appears to be ok out of the box.
 
2. Increase the participant resources to handle large messages

3. Install a  High Priority First Flow Controller on the participant.

   Also, shows how to tune the flow controller for a certain rate. This can
   be skipped/customized as needed.

4. Configure data writer to use the flow controller installed with the
   participant. Assigns a priority of 10 to the DataWriiter (for illustration).

   Also, customizes the sizes assuming large data samples will be sent. Those
   settings can be skipped if large data is not being used.

5. Configure data reader to match the large data customizations for the data writer.
   Those settings can be skipped if large data is not being used.
   
The example uses a modified HelloWorld example to show the High Priority First Flow Controller. 
The stock examples is modified as follows:

1. $NDDSHOME/example/CPP/Hello_idl/src/HelloWorld.idl is modified to increase the
         HELLODDS_MAX_PAYLOAD_SIZE = 8192*16; // 131MB
   so that we can easily test the the flow controller.

2. A HelloWorld example is generated in C++
     rtiddsgen -example x64Darwin10gcc4.2.1 HelloWorld.idl 

3. The generated HelloWorld_publisher.cxx is modified top add a line tagged with <<HPF>>
    instance->payload.ensure_length(HELLODDS_MAX_PAYLOAD_SIZE, HELLODDS_MAX_PAYLOAD_SIZE); //<<HPF>>

4. The USER_QOS_PROFILES.xml is modified to configure a High Priority First Flow controller.
 

References
----------
1. RTI_CoreLibrariesAndUtilities_UsersManual, Section 6.6
   FlowControllers (DDS Extension)

2. RTI_CoreLibrariesAndUtilities_UsersManual, Section 15.6.2 
   Setting the Maximum Gather-Send Buffer Count for UDPv4 and UDPv6

3. Tune Your OS for Performance: 
   http://community.rti.com/best-practices/tune-your-os-performance

