========================================
    Example Code - Batching example
========================================

Concept
-------
Batching can be used to decrease the amount of communication overhead associated
with the transmission and (in the case of reliable communication) 
acknowledgement of small samples, in order to increase throughput.

It specifies and configures the mechanism that allows Connext to collect 
multiple user data samples to be sent in a single network packet, to take 
advantage of the efficiency of sending larger packets and thus increase 
effective throughput.

Batching can be used to increase effective throughput dramatically for small
data samples. Throughput for small samples (size < 2048 bytes) is typically 
limited by CPU capacity and not by network bandwidth. Batching many smaller 
samples to be sent in a single large packet will increase network utilization 
and thus throughput in terms of samples per second.

In the other hand, 'Turbo Mode' is an experimental feature that uses an 
intelligent algorithm that automatically adjusts the number of bytes in a batch
at run time according to current system conditions, such as write speed 
(or write frequency) and sample size. This intelligence is what gives it the 
ability to increase throughput at high message rates and avoid negatively 
impacting message latency at low message rates

Example description
--------------------
PUBLISHER
This example shows the differences between batching and turbo mode. Both options
will send batch. This means that instead of sending each sample out on the wire
with its own header, the example will store the data in a batch until it is full 
and then sending the entire batch as one data packet. This saves on overhead 
taken up by headers and such because all the data in the batch shares the same 
header. 

We have two different publisher (two different profiles). The first of them, 
we will use the batch QoS setting. We can write specifically how many samples 
have the batch. You can use Wireshark to see this feature: 
    batchin_10_samples.pcap file package no. 98.

The other way, Turbo Mode can be used and the number of samples in the batch 
(if it is needed to use batching) will be chosen automatically. In this case 
the used batch is 15 samples. You can use Wireshark to see this feature:
    turbo_mode.pcap file package no. 76.

SUBSCRIBER
No change from default behavior except for a few modifications to load its 
corresponding profiles. 
