===========================================
 Example Code -- Persistent Storage
===========================================
In the file README_DATABASES.txt you can find help to configure the database
to this example.

Concept
-------
This example shows the differences between the persistent storage options. These
options can be classified in three:   
  
  - Durable Writer History (User's Manual section 12.3): The DURABILITY 
    QosPolicy controls whether or not, and how, published samples are stored by
    the DataWriter application for DataReaders that are found after the samples
    were initially written. The samples stored by the DataWriter constitute the
    DataWriter's history.
    
    Connext provides the capability to make the DataWriter history durable, by
    persisting its content in a relational database. This makes it possible for
    the history to be restored when the DataWriter restarts.
    
    The association between the history stored in the database and the 
    DataWriter is done using the virtual GUID.
  
  - Durable Reader State (User's Manual section 12.4): Durable reader state
    allows a DataReader to locally store its state in disk and remember the data
    that has already been processed by the application. When an application
    restarts, each DataReader configured to have durable reader state
    automatically reads its state from disk. Data that has already been
    processed by the application before the restart will not be provided to the
    application again.
    
    Connext requires a relational database to persist the state of a DataReader.
    This database is accessed using ODBC.
    
  - Persistence Service: Persistence Service is a Connext application that saves
    data samples to transient or permanent storage, so they can be delivered to
    subscribing applications that join the system at a later time-even if the
    publishing application has already terminated.
    
    Persistence Service runs as a separate application; you can run it on the
    same node as the publishing application, the subscribing application, or
    some other node in the network.
    
    When configured to run in PERSISTENT mode, Persistence Service can use the
    filesystem or a relational database that provides an ODBC driver. For each
    persistent topic, it collects all the data written by the corresponding
    persistent DataWriters and stores them into persistent storage.
    
    See the Persistence Service Release Notes for the list of platforms and
    relational databases that have been tested.
    
    When configured to run in TRANSIENT mode, Persistence Service stores the
    data in memory.
    
Example Description
-------------------
This example has been configured to show three different scenarios, you can see
them in the attached .pps file.

    The first one, using 'durable writer history', a writer sends a sample to a
reader, and stores it in the relational database. If the writer is turned off
and a new reader has been connected to the Global Data Space. When the writer 
reconnect, it recovers its history from the database, and sends the old and new
samples to the new reader, and the new ones to the old reader.

    The second scenario will show how to use 'durable reader state'. In this 
example, a writer send a sample to a reader which uses 'durable reader state'.
The reader store this sample in the relational database. This reader is 
restarted and a new reader appears. The writer send again the sample to the 
new reader but not to the old reader, because it already has that sample in its
relational database.

    In the last scenario, we will use the 'persistence service'. When this 
service is running, it will keep the sent samples in the Global Data Space even
if the writer is not running. So the new readers will take all the samples 
stored in the 'persistence service'.

