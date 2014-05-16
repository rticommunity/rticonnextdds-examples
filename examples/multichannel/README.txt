===========================================
 Example Code -- MultiChannel
===========================================

Purpose
=======
This example shows how to use multi-channel DataWriters. You will learn how to

- Use the STRINGMATCH filter
- Exercise the new 4.4 content-filtered topic (CFT) API
- Configure the MULTICHANNEL QosPolicy
- Observe the performance benefits of using a multi-channel DataWriter

PUBLISHER:
The Publisher publishes round-robin instances with Symbols "A" through "Z" on 8
different channels of a multi-channel DataWriter. By default, the XML profile is 
used to specify the QoS. On the create_datawriter() call, you can toggle between 
writer_qos and DATAWRITER_QOS_DEFAULT to switch between using code and using 
XML. 

SUBSCRIBER:
The Subscriber uses a ContentFilteredTopic (CFT) with the STRINGMATCH filter, 
then dynamically modifies the CFT to match "A,D", and later just "D". You will 
notice that the DataReader does not have to specify multiple multicast receive 
addresses, because it learns about the DataWriter's receive_addresses upon 
endpoint discovery and then receives on them. 

Wireshark Output
==============
See multichannel.pcap. Notice that data samples are only being sent on the 
channels that are necessary for delivering samples that pass the DataReader's 
filter.

In contrast, singlechannel.pcap shows the wire traffic when multichannel is not 
used and when the DataReader specifies a multicast.receive_address of 
239.255.0.1. Observe that all data samples are being sent on the wire 
(i.e., reader-side filtering), since there is only one channel.
