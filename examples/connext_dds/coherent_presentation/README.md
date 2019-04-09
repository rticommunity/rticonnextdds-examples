# Example Code: Coherent Presentation

## Concept
A publishing application can request that a set of data-sample changes be
propagated in such a way that they are interpreted at the receivers' side as a
cohesive set of modifications. In this case, the receiver will only be able to
access the data after all the modifications in the set are available at the
subscribing end.

This is useful in cases where the values are inter-related. For example, suppose
you have two data-instances representing the 'altitude' and 'velocity vector' of
the same aircraft. If both are changed, it may be important to ensure that
reader see both together (otherwise, it may erroneously interpret that the
aircraft is on a collision course).

## Example Description
This example illustrates how to use coherency QoS to force writes to be grouped
on the receiving side. Suppose the reader is tracking the state of an object
with many fields. If changes to fields occur relatively infrequently, it is
inefficient to send the entire state on every update. Instead, we send an ID for
the object, an ID for the field to be updated, and the new value.

If there are constraints on valid field states, then we may want multiple
updates to occur atomically. Coherent presentation at the instance level scope
allows us to do this. If there are additional dependencies *across* instances,
topic level scope makes these changes atomic to the reader.

This shows how coherent access can be used to ensure written samples are viewed
atomically; that is, all samples sent between begin_ and end_coherent changes
will be available before the reader is notified that there are samples to read.

In this example, the subscriber receives updates for six state fields, a-f.
Because we request coherent access at the topic level, on_data_available is not
triggered until the writer ends the coherent changes, so we get a sequence of
coherent samples rather than being notified as each sample arrives.

Note that coherency alone does *not* guarantee that observers will see updates
atomically, only that all samples will be available. For instance, if another
thread interrupts the DDS receiver thread after on_data_available has processed
only four updates, the interrupting thread will see an inconsistent state for
the objects.

Coherent presentation currently requires reliable communication. Additionally,
the reader history depth must be sufficiently large to store all samples
received during the coherent change group.
