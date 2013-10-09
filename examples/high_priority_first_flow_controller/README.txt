			High Priority First Flow Controller Example
			===========================================
				   Rajive Joshi 2013.10.08
				
				
DESCRIPTION	

Shows how to configure a High Priority First Flow Controller. The configuration is
summarized in:
     USER_QOS_PROFILES.xml

It shows the configuration of the following items, and all the key changes (from the
generated example) are documented as XML comments.

1. Increase the transport sizes
   - send and receive buffers
   - gather-send buffers to maximize message coalescing/batching for a destination
   
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
   


EXAMPLE

Uses a modified HelloWorld example to show the High Priority First Flow Controller. The 
stock examples is modified as follows
1. $NDDSHOME/example/CPP/Hello_idl/src/HelloWorld.idl is modified to increase the
         HELLODDS_MAX_PAYLOAD_SIZE = 8192*16; // 131MB
   so that we can easily test the the flow controller.

2. A HelloWorld example is generated in C++
     rtiddsgen -example x64Darwin10gcc4.2.1 HelloWorld.idl 

3. The generated HelloWorld_publisher.cxx is modified top add a line tagged with <<HPF>>
    instance->payload.ensure_length(HELLODDS_MAX_PAYLOAD_SIZE, HELLODDS_MAX_PAYLOAD_SIZE); //<<HPF>>

4. The USER_QOS_PROFILES.xml is modified to configure a High Priority First Flow controller.

5. To Build:

   cd HighPriorityFirstFlowController/
   
   Generate makefile/project if not already present:
       rtiddsgen -makefile <your_platform> HelloWorld.idl 
   Built it
           make -f makefile_HelloWorld_ <your_platform>
        or execute the Windows project
        
   Example: 
        make -f makefile_HelloWorld_x64Darwin10gcc4.2.1 

6. To Run:
   In a shell window:
     ./objs/x64Darwin10gcc4.2.1/HelloWorld_publisher 
 
   In another shell window:
     ./objs/x64Darwin10gcc4.2.1/HelloWorld_subscriber 
 

REFERENCES
1. RTI_CoreLibrariesAndUtilities_UsersManual, Section 6.6
   FlowControllers (DDS Extension)

2. RTI_CoreLibrariesAndUtilities_UsersManual, Section 15.6.2 
   Setting the Maximum Gather-Send Buffer Count for UDPv4 and UDPv6

3. Tune Your OS for Performance: 
   http://community.rti.com/best-practices/tune-your-os-performance
 