===========================================
 Example Code -- Routing Service File Adapter
===========================================

Concept
-------
Adapters are pluggable components that allow RTI Routing Service to consume and
produce data for different data domains (e.g., DDS, JMS, Socket, etc.).

By default, RTI Routing Service is distributed with a built-in DDS adapter. Any other 
adapter plugins must be provided as a set of Java classes registered within the 
<adapter_library> tag.

Input adapters are used to collect data samples from different data domains, 
such as Connext or JMS. The input samples are processed by the Routing Service 
engine and are passed along to custom transformations if they are defined. 
Finally, the transformed data is provided to the output adapters. The adapter plugin 
API is used to create new adapters; it is supported in C and Java.

Example Description
-------------------
This example shows how to create a simple adapter using the RTI Routing Service
Adapter SDK. The adapter scans the file system in one specific folder defined in
the XML configuration file. For every file present in that folder, it creates a stream 
to communicate with the output. The data that will flow in this stream are the 
lines inside the files. In summary, we have a stream for every file in the 
specified folder, and every stream transport on the other side the text lines 
contained inside the specific file. In the output, the adapter will take care of
creating a file for every stream received. The files will be created on a specific
path, which you can also configure in the XML configuration file.
