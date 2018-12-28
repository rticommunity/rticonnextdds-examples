============================================
 Example Code -- Creating a Custom Transport
============================================

Concept
-------

RTI Connext DDS interacts with the underlying network/communications transport via a 
RTI Pluggable Transport API (PT API).  
The PT API sits in the protocol stack below the DDS Protocol (DDS-RTPS) layer. 
It is used for sending and receiving all the information between DomainParticipants. 

On the sending side the PT API gets passed from DDS full RTPS packets as an array of 
bytes along with the intended destination (address / port) for the packet.  
On the receiving side the PT API gets these packets using its own internal transport
mechanism and gives them to the DDS-RTPS layer above.

+----------------------------+
|                            |
|         <Application>      |
|                            |
+............................+
|           DDS API          |
|                            |
|       <Connext DDS >       |
+............................+
|   Pluggable Transport API  |
|                            |
| <Transport Implementation> |
+----------------------------+

RTI Connext DDS uses the PT API to implement all the supported transports bundled in the
standard distribution, including UDP/IP, TCP/IP, TLS, DTLS, Shared Memory, etc.  

Application developers can use the PT API to develop custom transports. 
Custom transports must be developed in C and hey can be either linked to the final 
application or dynamically loaded. The configuration can be done from the
application code or using file-based XML QoS Profiles.

The documentation of the pluggable transport API can be found at:
http://community.rti.com/rti-doc/500/RTI_Transport_Plugin_5.0.0/doc/html/index.html

The following XML snippet illustrates the XML configuration used by the application 
in this example. It loads the library named "rti_custom_transports" which contains
the "FileTransport". FileTransport is a transport plugin implemented in the files 
FileTransport.c and FileTransport.h included as part of this example.

Pluggable transports can be loaded and configured via the DomainParticipant PropertyQoS.
As seen in the file below the property "dds.transport.load_plugins" is used to specify
a new transport plugin class and plugin instance. In this example "FILE.myPlugin".
Once a plugin name has been specified, the properties with prefix 
"dds.transport.myFilePlugin" (where myFilePlugin stands for whatever name was specified
in the dds.transport.load_plugins property) are used to configure that plugin.

For example in the file below the property "dds.transport.FILE.myPlugin.library" is
used to configure the dynamic library that contains the plugin implementation,
"dds.transport.FILE.myPlugin.create_function" defines the entry function into the
library that creates the plugin, etc.

        <qos_profile name="custom_transport_Profile" is_default_qos="true">
            <participant_qos>
                <!-- disable all builtin transports -->
                <transport_builtin>
                    <mask>MASK_NONE</mask>
                </transport_builtin>
                <property>
                    <value>
                      <element>
                            <name>dds.transport.load_plugins</name>
                            <value>dds.transport.FILE.myPlugin</value>
                            <propagate>0</propagate>
                        </element>
                       <element>
                            <name>dds.transport.FILE.myPlugin.library</name>
                            <value>rti_custom_transports</value>
                            <propagate>0</propagate>
                        </element>
                       <element>
                            <name>dds.transport.FILE.myPlugin.create_function</name>
                            <value>NDDS_Transport_FILE_create</value>
                            <propagate>0</propagate>
                        </element>                      
                        <element>
                            <name>dds.transport.FILE.myPlugin.address</name>
                            <value>3.3.3.3</value>
                            <propagate>0</propagate>
                        </element>   
                         <element>
                            <name>dds.transport.FILE.myPlugin.trace_level</name>
                            <value>1</value>
                            <propagate>0</propagate>
                        </element>   
 
                      </value>
                </property>
            </participant_qos>
        </qos_profile>



Example description
-------------------

The FILE Transport-Plugin was developed as an example of how to use the
the transport plugin API.  The emphasis is on simplicity and understandability,
not performance or scalability.

There has been no effort to make this a "real" transport to be used in a real
system. Rather the goal was to use simple APIs for communications that would 
not distract from the APIs required to interface with the Connext DDS Core.

The FILE transport uses regular files for communication. Each "address/port" maps
to a file in the file-system. The location of these files is configurable. By
default they are placed under "/tmp/dds/FileTransport"  so the file used to
receive on address "10.10.10.10" port 9999 would be: 
"/tmp/dds/FileTransport/10.10.10.10/9999"

The transport was implemented for Unix systems (Linux, MacOSX) and uses 
Unix system calls to perform file operations such as locking, reading and
writing. It should be possible to port the FileTransport other operating systems
as long as they offer similar functionality.
 
The address of each DomainParticipant that uses the transport is configured using
PropertyQos in the DomainParticipant as shown in the XML snippet above. 
In the snippet above the selected address is "3.3.3.3". The addresses must be given 
as IPv4 (or IPv6) addresses using the standard dot notation. The addresses will be 
mapped to file-names by the transport plugin. If no address is
specified in the PropertyQos the default of "1.1.1.1" is used.

Multiple DomainParticipants can communicate using the FileTransport. 
The DomainParticipants may run on different computers as
long as they have access to some common file systems. 

DomainParticipants may also be configured to have different "addresses" regardless
of weather they run on the same computer. The FileTransort only uses the address 
to construct the filename.

It is also possible to have a single participant can instantiate more then one 
FileTransport, if so, each should be given a different name and network address.

Communication using the FileTransport is simple. To send to an address/port 
the transport opens the corresponding file and appends the data to the end of the file. 
To receive, the FileTransport opens the file and reads form it, keeping track of the 
last position read so that it can keep reading the new data.

Access to the files is serialized using file locking. Because of this multiple
applications can send "simultaneously" to the same address/port and the
bytes written are serialized in the corresponding file. The receiver also
uses file-locking to ensure it does not read partial messages from a file that 
is being concurrently updated.
 
The "receive" operation on the TransportPlugin is required to block until
data is available. To keep things simple the FileTransport implements
the blocking behavior by polling the file at a 1 second period to check
see if there were any changes to it. This is obviously not efficient
but it keeps the code simple which is the main goal.

Custom transports must be implemented in C. Please refer to the "c" subdirectory 
to find the source files" FileTransport.c and FileTransport.h that implement the 
transport along with instructions on how to build and run.

