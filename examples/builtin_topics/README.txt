===============================
Example Code - Builtin Topics
===============================

Purpose 
======
PUBLISHER: 
This publisher example shows how to attach listeners to Builtin Topics. Each participant has a builtin subscriber with three topics. These subscriptions receive data when the participant gets information about another participant, datareader or datawriter. 

Listening to these topics has several uses. For instance, we can see discovery information, read user_data, or check the typecodes other entities are using. 

This example shows how user_data can be used for security. We check user_data fields for discovered participants and datareaders. If the participant user_data matches an authorization string, we allow all datareaders from that participant read access. Otherwise, we check the user_data from individual datareaders to see if it is authorized. 

This exposes two points. First, even though the builtin topics correspond to a hierarchy of participants -> datawriters/datareaders, there is no hierarchy of listener callbacks. If, as in this example, we listen for participant and datareader information, both listeners will get the on_data_available() callbacks. If we want to track relationships between discovered participants and readers/writers, we may do so by keys. 

Second, the callbacks for participant discovery are guaranteed to fire before the callbacks for readers or writers that are children of that participant. 


SUBSCRIBER: 
This shows how to set the user_data qos fields for participants and data readers. To do this, we first get the relevant default qos, then add our data as an DDS_OctetSeq. These Octets are opaque to DDS, and will not undergo any conversions during transmission. 

