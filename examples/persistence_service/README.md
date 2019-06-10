# Persistence Service

Persistence Service is a Connext DDS application that saves DDS data samples to
transient or permanent storage, so they can be delivered to subscribing
application has already terminated.

Persistence Service runs as a separate application; you can run it on the same
node as the publishing application, the subscribing application, or some other
node in the network.

When configured to run in PERSISTENT mode, Persistence Service can use the
filesystem or a relational database that provides an ODBC driver. For each
persistent topic, it collects all the data written by the corresponding
persistent DataWriters and stores them into persistent storage. See the RTI
Persistence Service Release Notes for the list of platforms and relational
databases that have been tested.

When configured to run in TRANSIENT mode, Persistence Service stores the data
in memory.

In this directory you will find some examples about the Persistence Service.
For more info check out the
[documentation](https://community.rti.com/static/documentation/connext-dds/6.0.0/doc/manuals/connext_dds/html_files/RTI_ConnextDDS_CoreLibraries_UsersManual/index.htm#UsersManual/PersistenceIntro.htm).
