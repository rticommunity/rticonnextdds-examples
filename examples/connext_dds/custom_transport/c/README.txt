============================================
 Build a Custom Transport
============================================

The example FileTransport plugin is contained in the files: FileTransport.c 
and FileTransport.h

Before compiling or running the example, make sure the environment variable 
NDDSHOME is set to the directory where your version of RTI Connext is installed.


Building the Custom Transport dynamic library
---------------------------------------------

The accompanying makefile_custom_transport_plugins_x64Darwin10gcc4.2.1.mk can be used to build
the dynamic library in MacOSX for the x64Darwin10gcc4.1.1 platform.  
Similarly the makefile makefile_custom_transport_plugins_i86Linux2.6gcc4.1.1.mk 
can be used for the linux i86Linux2.6gcc4.1.1 platform.

To build the transport dynamic library on MaxOSX run:
make -f makefile_custom_transport_plugins_x64Darwin10gcc4.2.1.mk

To build the transport dynamic library on Linux run:
make -f makefile_custom_transport_plugins_i86Linux2.6gcc4.1.1.mk


Building the "C" Example that uses the Custom Transport dynamic library
-----------------------------------------------------------------------

Run rtiddsgen with the -example option and the target architecture of your 
choice (e.g. x64Darwin10gcc4.2.1, and i86Linux2.6gcc4.4.3). The RTI Connext Core 
Libraries and Utilities Getting Started Guide describes this process in detail. 
Follow the same procedure to generate the code and build the examples. Do not 
use the -replace option.

On MacOSX systems (assuming you want to generate an example for 
ix64Darwin10gcc4.2.1) run:

rtiddsgen -language C -example x64Darwin10gcc4.2.1 custom_transport.idl

On UNIX systems (assuming you want to generate an example for i86Linux2.6gcc4.1.1).

makefile_custom_transport_plugins_i86Linux2.6gcc4.1.1i86Linux2.6gcc4.4.3) run:

rtiddsgen -language C -example i86Linux2.6gcc4.1.1 custom_transport.


You will see some messages that indicate that the USER_QOS_PROFILES.xml was 
already existing and was not overiden. In this situation it is exactly what we need.

Running the "C" the Example that uses the Custom Transport dynamic library
---------------------------------------------------------------------------

When custom_transport_publisher and custom_transport_subscriber are run they 
read the USER_QOS_PROFILES.xml file
located in the directory from which the application is run. This file,
a snippet of which appears above, instructs DDS to load the transport library
built from the FileTransport.c and FileTransport.h and use it for all communications.

In order to run the applications you will need to do the following:

(1) Build the publisher and subscriber applications. 
On MacOSX you can do that with the command:
make -f makefile_custom_transport_x64Darwin10gcc4.2.1.mk

(2) Build the transport plugins library. 
On MacOSX you can do that with the command:
make -f makefile_custom_transport_plugins_x64Darwin10gcc4.2.1.mk

(3) Set the load library path to include both the Connext DDS libraries and 
the newly created library.

On MacOSX you can do that with the command:
export DYLD_LIBRARY_PATH=./objs/x64Darwin10gcc4.2.1:${NDDSHOME}/lib/x64Darwin10gcc4.2.1

On Linux you do it with the command:
export LD_LIBRARY_PATH=./objs/i86Linux2.6gcc4.1.1:${NDDSHOME}/lib/i86Linux2.6gcc4.1.1


(4) Note that the USER_QOS_PROFILES.xml configures the NDDS_DISCOVERY_PEERS using the
participant discovery qos. The XML snippet:
               <discovery>
                    <initial_peers>
                        <element>FileTransport://3.3.3.3</element>
                    </initial_peers>
                </discovery>
 
Tells the participant to find its peers using the FileTransport (which is the name we 
assigned to this transport) at address 3.3.3.3 (which is the one we configured for the
participant using the property "dds.transport.FILE.myPlugin.address"

if we wanted to configure participants to have multiple addresses (e.g. 3.3.3.3 and 2.2.2.2),
then we should also add all these addresses to the list of discovery peers on all 
the participants:

               <discovery>
                    <initial_peers>
                        <element>FileTransport://3.3.3.3</element>
                        <element>FileTransport://2.2.2.2</element>
                    </initial_peers>
                </discovery>

(5) Run the publisher and subscribing applications
You should get output similar to this:

ON THE PUBLISHER:
./objs/x64Darwin10gcc4.2.1/custom_transport_publisher 
NDDS_Transport_FILE_newI: pid: 22834 bound to address: "3.3.3.3"
NDDS_Transport_FILE_get_class_name_cEA: FifoTransport
NDDS_Transport_FILE_get_receive_interfaces_cEA: num receive interfaces: 1
    interface[0] = "3.3.3.3" (0000:0000:0000:0000:0000:0000:0303:0303)
NDDS_Transport_FILE_open_file_for_port (port 7410): initialized file=0x100d6bf60, fileName: "/tmp/dds/FileTransport/3.3.3.3/7410"
NDDS_Transport_FILE_create_recvresource_rrEA: created receive resource for address= "3.3.3.3", port= 7410
NDDS_Transport_FILE_get_receive_interfaces_cEA: num receive interfaces: 1
    interface[0] = "3.3.3.3" (0000:0000:0000:0000:0000:0000:0303:0303)
NDDS_Transport_FILE_share_recvresource_rrEA: resource at port '7410' not reused for port 7411
NDDS_Transport_FILE_open_file_for_port (port 7411): initialized file=0x100d6bff8, fileName: "/tmp/dds/FileTransport/3.3.3.3/7411"
NDDS_Transport_FILE_create_recvresource_rrEA: created receive resource for address= "3.3.3.3", port= 7411
NDDS_Transport_FILE_get_receive_interfaces_cEA: num receive interfaces: 1
    interface[0] = "3.3.3.3" (0000:0000:0000:0000:0000:0000:0303:0303)
NDDS_Transport_FILE_get_receive_interfaces_cEA: num receive interfaces: 1
    interface[0] = "3.3.3.3" (0000:0000:0000:0000:0000:0000:0303:0303)
NDDS_Transport_FILE_create_sendresource_srEA: Send resource 0x100f5fb70 (file ="/tmp/dds/FileTransport/2.2.2.2/7412") created for port= 7412, address= 2.2.2.2 (0000:0000:0000:0000:0000:0000:0202:0202)
NDDS_Transport_FILE_create_sendresource_srEA: Send resource 0x100f5fc10 (file ="/tmp/dds/FileTransport/2.2.2.2/7410") created for port= 7410, address= 2.2.2.2 (0000:0000:0000:0000:0000:0000:0202:0202)
NDDS_Transport_FILE_create_sendresource_srEA: Send resource 0x100f5fd60 (file ="/tmp/dds/FileTransport/3.3.3.3/7412") created for port= 7412, address= 3.3.3.3 (0000:0000:0000:0000:0000:0000:0303:0303)
NDDS_Transport_FILE_create_sendresource_srEA: Send resource 0x100f5fe00 (file ="/tmp/dds/FileTransport/3.3.3.3/7410") created for port= 7410, address= 3.3.3.3 (0000:0000:0000:0000:0000:0000:0303:0303)
Writing CustomTransport, count 0
Writing CustomTransport, count 1
Writing CustomTransport, count 2
NDDS_Transport_FILE_create_sendresource_srEA: Send resource 0x101b17270 (file ="/tmp/dds/FileTransport/3.3.3.3/7413") created for port= 7413, address= 3.3.3.3 (0000:0000:0000:0000:0000:0000:0303:0303)
Writing CustomTransport, count 3
Writing CustomTransport, count 4
Writing CustomTransport, count 5
Writing CustomTransport, count 6


ON THE SUBSCRIBER:
./objs/x64Darwin10gcc4.2.1/custom_transport_subscriber 
NDDS_Transport_FILE_newI: pid: 22836 bound to address: "3.3.3.3"
NDDS_Transport_FILE_get_class_name_cEA: FifoTransport
NDDS_Transport_FILE_get_receive_interfaces_cEA: num receive interfaces: 1
    interface[0] = "3.3.3.3" (0000:0000:0000:0000:0000:0000:0303:0303)
NDDS_Transport_FILE_open_file_for_port: file '/tmp/dds/FileTransport/3.3.3.3/7410' already exists
NDDS_Transport_FILE_create_recvresource_rrEA: failed to create receive resource for address= "3.3.3.3", port= 7410
NDDS_Transport_FILE_get_receive_interfaces_cEA: num receive interfaces: 1
    interface[0] = "3.3.3.3" (0000:0000:0000:0000:0000:0000:0303:0303)
NDDS_Transport_FILE_open_file_for_port (port 7412): initialized file=0x100d6df60, fileName: "/tmp/dds/FileTransport/3.3.3.3/7412"
NDDS_Transport_FILE_create_recvresource_rrEA: created receive resource for address= "3.3.3.3", port= 7412
NDDS_Transport_FILE_get_receive_interfaces_cEA: num receive interfaces: 1
    interface[0] = "3.3.3.3" (0000:0000:0000:0000:0000:0000:0303:0303)
NDDS_Transport_FILE_share_recvresource_rrEA: resource at port '7412' not reused for port 7413
NDDS_Transport_FILE_open_file_for_port (port 7413): initialized file=0x100d6dff8, fileName: "/tmp/dds/FileTransport/3.3.3.3/7413"
NDDS_Transport_FILE_create_recvresource_rrEA: created receive resource for address= "3.3.3.3", port= 7413
NDDS_Transport_FILE_get_receive_interfaces_cEA: num receive interfaces: 1
    interface[0] = "3.3.3.3" (0000:0000:0000:0000:0000:0000:0303:0303)
NDDS_Transport_FILE_get_receive_interfaces_cEA: num receive interfaces: 1
    interface[0] = "3.3.3.3" (0000:0000:0000:0000:0000:0000:0303:0303)
NDDS_Transport_FILE_create_sendresource_srEA: Send resource 0x100f4e380 (file ="/tmp/dds/FileTransport/2.2.2.2/7412") created for port= 7412, address= 2.2.2.2 (0000:0000:0000:0000:0000:0000:0202:0202)
NDDS_Transport_FILE_create_sendresource_srEA: Send resource 0x100f4e420 (file ="/tmp/dds/FileTransport/2.2.2.2/7410") created for port= 7410, address= 2.2.2.2 (0000:0000:0000:0000:0000:0000:0202:0202)
NDDS_Transport_FILE_create_sendresource_srEA: Send resource 0x100f4e570 (file ="/tmp/dds/FileTransport/3.3.3.3/7412") created for port= 7412, address= 3.3.3.3 (0000:0000:0000:0000:0000:0000:0303:0303)
NDDS_Transport_FILE_create_sendresource_srEA: Send resource 0x100f4e610 (file ="/tmp/dds/FileTransport/3.3.3.3/7410") created for port= 7410, address= 3.3.3.3 (0000:0000:0000:0000:0000:0000:0303:0303)
CustomTransport subscriber sleeping for 4 sec...
NDDS_Transport_FILE_create_sendresource_srEA: Send resource 0x1019468f0 (file ="/tmp/dds/FileTransport/3.3.3.3/7411") created for port= 7411, address= 3.3.3.3 (0000:0000:0000:0000:0000:0000:0303:0303)

   str_value: "Iteration: 3"
   long_value: 3
CustomTransport subscriber sleeping for 4 sec...

   str_value: "Iteration: 4"
   long_value: 4
CustomTransport subscriber sleeping for 4 sec...

   str_value: "Iteration: 5"
   long_value: 5
CustomTransport subscriber sleeping for 4 sec...

   str_value: "Iteration: 6"
   long_value: 6

