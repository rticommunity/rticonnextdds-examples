# Example Code: Partitions

## Concept
The *PARTITION QoS* provides another way to control which *DataWriters* will
match, and thus communicate with, which *DataReaders*. It can be used to
prevent *DataWriters* and *DataReaders* that would have otherwise matched with
the same *Topic* and compatible *QosPolicies* from talking to each other. Much
in the same way that only applications within the same domain will communicate
with each other, only *DataWriters* and *DataReaders* that belong to the same
partition can talk to each other.

The *PARTITION QoS* consists of a set of partition names that identify the
partitions of which the *Entity* is a member. These names are simply strings,
and *DataWriters* and *DataReaders* are considered to be in the same partition
if they have more than one partition name in common in the *PARTITION
QoS* set on their *Publishers* or *Subscribers*.

## Example description
In the Publisher application, we change the Partition name list every 5
samples. Our purpose is to show how literals and regular expressions
can be used to match the subscriber's name list.

In the subscriber application, we create a reliable reader, and request
old samples upon joining, showing  that partitions are subscriber-based,
not data-based. If we start the subscriber late, it will receive all samples
available, even those that were published while under incompatible partitions.

Furthermore, if the reader and writer unpair due to a partition mismatch, the
reader will interpret the instance as new if it reconnects.
