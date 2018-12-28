# Example Code: MultiChannel

## Concept
The `MULTI_CHANNEL` Qos policy is used to partition the data published by a
*DataWriter* across multiple channels. A channel is defined by a filter
expression and a sequence of multicast locators.

By using this Qos policy, a *DataWriter* can be configured to send data to
different multicast groups based on the content of the data. Using syntax
similar to those used in *Content-Based Filters*, you can associate different
multicast addresses with filter expressions that operate on the values of the
fields within the data. When your application's code calls `write()`, data is
sent to any multicast address for which the data passes the filter.

## Example Description
This example shows how to use multichannel *DataWriters*. You will learn how to:

* Use the `STRINGMATCH` filter.
* Exercise the *Content-Filtered Topic* (CFT) API.
* Configure the `MULTICHANNEL` Qos policy.
* Observe the performance benefits of using a multichannel *DataWriter*.

The Publisher application publishes round-robin instances with Symbols `A`
through `Z` on 8 different channels of a multichannel *DataWriter*. By default,
the XML profile is used to specify the QoS. On the `create_datawriter()` call,
you can toggle between `writer_qos` and `DATAWRITER_QOS_DEFAULT` to
switch between using code and using XML.

The Subscriber application uses a *Content-Filtered Topic* (CFT) with the
`STRINGMATCH` filter, then dynamically modifies the CFT to match `A,D`,
and later just `D`. You will notice that the *DataReader* does not have to
specify multiple multicast receive addresses, because it learns about the
*DataWriter's* `receive_addresses` upon endpoint discovery and then receives
on them.

The example includes also two PCAP files captured with *Wireshark* that
show the traffic using multichannel and single channel.

* multichannel.pcap shows the wire traffic when multichannel is used.
  Notice that data samples are only being sent on the channels that
  are necessary for delivering samples that pass the *DataReader's* filter.

* singlechannel.pcap shows the wire traffic when multichannel is not
  used and when the *DataReader* specifies a `multicast.receive_address` of
  `239.255.0.1`. Observe that all data samples are being sent on the wire
  (i.e., reader-side filtering), since there is only one channel.
