===============================
Example Code - Builtin Topics
===============================

Concept
-------
Built-in Topics are a special kind of topics that are used by RTI Connext DDS
applications to discover each other. These Topics are handled automatically by
the middleware, but in certain scenarios it is useful to access to them. For
instance, these Topics allow us to access some relevant information about
Connext entities, such as:

- The publications and subscriptions available
- The TypeCode of publications
- The QoS requested/offered by DataReaders and DataWriters.
- Access to application specific data in discovery (e.g. USER_DATA)


Example description
--------------------

This example shows how USER_DATA QoS can be used for security during discovery
phase. For this purpose, we check USER_DATA fields for discovered Participants
and datareaders. If the Participant USER_DATA matches an authorization string,
we allow all DataReaders from that Participant read access. Otherwise, we check
the USER_DATA from individual DataReaders to see if it is authorized. 

PUBLISHER: 
This Publisher example shows how to attach listeners to Builtin Topics. Each
Participant has a builtin Subscriber with three Topics. These subscriptions
receive data when the Participant gets information about another Participant,
DataReader or DataWriter. 

This exposes two points. First, even though the builtin Topics correspond to a
hierarchy of Participants -> DataWriters/DataReaders, there is no hierarchy of
listener callbacks. If, as in this example, we listen for Participant and
DataReader information, both listeners will get the on_data_available()
callbacks. If we want to track relationships between discovered participants and
DataReaders/DataWriters, we may do so by keys. 

Second, the callbacks for Participant discovery are guaranteed to fire before
the callbacks for readers or writers that are children of that Participant. 

SUBSCRIBER: 
This shows how to set the user_data qos fields for participants and data
readers. To do this, we first get the relevant default qos, then add our data as
 an DDS_OctetSeq. These Octets are opaque to DDS, and will not undergo any
 conversions during transmission. 

