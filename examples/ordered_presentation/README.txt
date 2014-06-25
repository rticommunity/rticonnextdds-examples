======================================
 Example Code -- Ordered presentation
======================================

Concept
-------
Usually DataReaders will receive data in the order that it was sent by a 
DataWriter. In addition, data is presented to the DataReader as soon as the 
application receives the next value expected.

Sometimes, you may want a set of data for the same DataWriter to be presented to
the receiving DataReader only after ALL the elements of the set have been 
received, but not before. You may also want the data to be presented in a 
different order than it was received. Specifically, for keyed data, you may want
Connext to present the data in keyed or instance order.

If we use Ordered Presentation, the access_scope controls the scope of the order
in which samples are presented to the subscribing application. The access_scope
may have four different values:

    - If access_scope is INSTANCE, the relative order of samples sent by a 
      DataWriter is only preserved on an per-instance basis. If two samples 
      refer to the same instance (identified by Topic and a particular value for
      the key) then the order in which they are stored in the DataReader’s queue
      is consistent with the order in which the changes occurred. However,if the
      two samples belong to different instances, the order in which they are 
      presented may or may not match the order in which the changes occurred.

    - If access_scope is TOPIC, the relative order of samples sent by a 
      DataWriter is preserved for all samples of all instances. The coherent 
      grouping and/or order in which samples appear in the DataReader’s queue is
      consistent with the grouping/order in which the changes occurred—even if 
      the samples affect different instances.
      
    - If access_scope is GROUP, the scope spans all instances belonging to 
      DataWriter entities within the same Publisher—even if they are instances 
      of different topics. Changes made to instances via DataWriter entities 
      attached to the same Publisher are made available to Subscribers on the 
      same order they occurred.
    
    - If access_scope is HIGHEST_OFFERED, the Subscriber will use the access 
      scope specified by each remote Publisher.

Example Description
-------------------
In this example we illustrate how to control the scope of the order in which
samples are presented to the subscribing application.

The Publisher sets its presentation QoS properties to enable topic-level ordered
access. However, note that this only enforces ordering on instances from a given 
datawriter, and not from all writers of that topic. Also note that ordered-
access configuration needs to be configured in the subscription side makes so 
samples are read in an ordered manner. 

The Subscriber application illustrates the effects of the access_scope 
presentation QoS, to creates two DDS subscribers. Subscriber 0 uses instance 
scope, meaning changes and samples are only guaranteed to be ordered *within* a 
given instance. Subscriber 1 uses topic scope, which enforces order *across* 
instances that are modified by the same datawriter. 

Note that using instance access_scope does not guarantee that samples will be 
read or taken from the queue in a particular manner. Subscriber 0 returns all 
of Instance 0's samples, then all of Instance 1's samples. However, it would 
also be valid to return samples in the same order as Subscriber 1. To guarantee
that samples from the same instance are return together, use the 
take/read_*instance calls. See the online API for FooDataReader for more 
information. 
