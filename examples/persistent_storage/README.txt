===========================================
 Example Code -- Persistent Storage
===========================================

Concept
-------
In many applications, we need to store data samples so they can be delivered to
subscribing applications that join the system at a later time or to avoid
receiving old data each time we re-join to the same publisher. Or maybe we need
to keep the data available even once our publishing application has terminated.
In RTI Connext DDS there are three different approaches to store data 
persistently:

- Durable Writer History
The DURABILITY QosPolicy controls whether or not, and how, published samples are
stored by the DataWriter application. Connext provides the capability to make 
the DataWriter history durable, by persisting its content in a relational 
database. This makes it possible for the history to be restored when the 
DataWriter restarts.

The association between the history stored in the database and a DataWriter is 
done using the virtual GUID. This way, those DataReaders found after the samples
were initially written will receive all the stored samples as if the original 
DataWriter were sending them.

- Durable Reader State
Durable reader state allows a DataReader to locally store its state in disk and
remember the data that has already been processed by the application. When an
application restarts, each DataReader configured to have durable reader state
automatically reads its state from disk. Data that has already been processed by
the application before the restart will not be provided to the application 
again.

Connext requires a relational database to persist the state of a DataReader. 
This database is accessed using ODBC.

- Persistence Service
Persistence Service is a Connext application that saves data samples to 
transient or permanent storage, so they can be delivered to subscribing 
applications that join the system at a later time -even if the publishing 
application has already terminated.

Persistence Service runs as a separate application; you can run it on the same 
node as the publishing application, the subscribing application, or some other 
node in the network.

When configured to run in PERSISTENT mode, Persistence Service can use the 
filesystem or a relational database that provides an ODBC driver. For each 
persistent topic, it collects all the data written by the corresponding 
persistent DataWriters and stores them into persistent storage.See the 
Persistence Service Release Notes for the list of platforms and relational 
databases that have been tested.

When configured to run in TRANSIENT mode, Persistence Service stores the data 
in memory.

Example Description
-------------------
This example shows the three different scenarios explained above.

- Durable Writer History
    1. A DataWriter sends some samples to a DataReader, and stores them in the 
       relational database.
    2. Then, we delete that DataWriter, keeping data stored thanks to the Data
       Writer History functionality. After that a new DataReader joins.
    3. At this point, we restart the original DataWriter (assigning the same 
       GUID), which recovers its history from the database.
    4. This causes that the DataWriter sends the old and new samples to the new
       DataReader, but only the new ones to the old DataReader.

- Durable Reader State
    1. In this scenario, a DataWriter sends samples to a DataReader which stores
       them in a relational database.       
    2. Then, we restart this DataReader (assigning the same GUID) and create a 
       second DataReader. The DataWriter discovers the two DataReaders, but only
       sends the samples to the new DataReader. Avoiding sending it to the first
       DataReader which already had them in its relational database.
       
- Persistence Service
    1. When Persistence Service is running, it stores all the samples sent.
    2. Then when a new DataReader joins, Persistence Service re-sends them to 
       the DataReaders, even if the DataWriter is no longer alive.


Note
----
This file explains different methods to storage data persistently. For details 
on configuring the databases needed for some of these methods, refer to the file
README_DATABASES.txt.
