===========================================
 Example Code -- Keyed data: Advanced
===========================================

Concept
-------
This is a second example that shows some advanced concepts related to keyed data.
As this example builds on 'Keyed Data' example (available under 
http://community.rti.com/examples/keyed-data), it is advisable to understand 
that example before investigating this one. 

The OWNERSHIP Qos Policy specifies whether a DataReader receive data for an 
instance of a Topic sent by multiple DataWriters. This example explores how 
DataWriters and DataReaders associated to keyed Topics interact with the OWNERSHIP 
QoS policy.


Example Description
-------------------
In this example, we will show how the Ownership QoS interacts with the 
registration, unregistration, and disposal of instances of multiple 
DataWriters. 

We will explore how the on_data_available callback on the DataReader can 
delegate to other handlers based on instance state changes. 

We will also show how to read only certain keys. Note that this is an 
entirely separate mechanism than content based filters. "Filtering" in 
this case is done on the DataReader side, and thus saves no network overhead. 
An example of when such a scheme may be useful is when a DataReader is receiving 
many samples via multicast. After filtering in on_data_available, the rest of 
the application does not need to deal with keys it doesn't care about. 

The example creates two DataWriters:
 A) DW1 : It writes positive numbers. Has a OWNERSHIP strenght of 10.
 B) DW2 : It writes negative numbers. Has a OWNERSHIP strenght of 5.

These DataWrites have the following behavior: 
 - At t=0 DW1 starts continously writting to an instance (ins0).

 - At t=0 DW2 starts continously writting to an instance (ins1).

 - At t=4 DW1 registers two instances (ins1 and ins2).
   Since DW1 has the greater OWNERSHIP strength, only its ins1 updates are delivered to the DR.

 - At t=8 DW1  disposes ins1.
   Since DW1 has not unregistered the instance, the DR does not receive ins1 updates from DW2.

 - At t=10 DW1 unregisters ins1.
   The DR starts receiving ins1 updates from DW2.

 - At t=12 DW1 unregisters ins2 and re-registers ins1.
   The DR receives ins1 updates from DW1.

 - At t=16 DW1 re-registers ins2 and DW2 disposes ins1.







