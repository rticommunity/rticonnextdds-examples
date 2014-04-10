===========================================
 Example Code -- Asynchronous Publisher
===========================================

Concept
-------
This Example shows how to create a simple adapter for RTI Routing Service. An adapter for Routing Service comes with the xml configuration file, and with the code of the adapter. RTI Routing Service comes with a builtin adapter, that you can use in case you just need to route your sample from on DDS domain to another. An adapter for RTI Routing Service is necessary to connect Routing Service to other representation, for instance with the file system, JMS or a socket. You can always create an adapter for Routing Service if you want to route samples from one DDS Domain to another and you want a different behavior from the default. 

Example Description
-------------------

In this example the adapter for RTI Routing Service doesn't work at all with DDS Domain, it scans a folder specified inside the xml configuration file, for every new file that appears in the specified folder, discovery reader gets notified, and a new stream gets created. The data routed inside the stream, are the content of the file. On the writer side side, according to the write mode policy chosen in the xml configuration file, a new file gets created and data gets written in it, or data can be appended on an existing file. 
