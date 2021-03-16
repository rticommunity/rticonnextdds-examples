# Example Code: Group Coherent Presentation

## Concept

A publishing application can request that a set of data-sample changes across
multiple DataWriters be propagated in such a way that they are interpreted at
the receivers' side as a cohesive set of modifications. In this case, the
matching DataReaders will only be able to access the data after all the
modifications in the set are available at the subscribing end.

This is useful in cases where the values are inter-related. For example, suppose
you have data about a patient in a hospital, such as their heart rate and
temperature. As you are monitoring the patient, if either of these vitals reads
in the abnormal range, an alarm is sounded. When the alarm goes off, we want the
patient's vitals, along with the alarm data, to be received as a coherent set
so that we can interpret all of the data as a cohesive unit. Otherwise, we may
incorrectly attribute an older (or more recent) temperature/heart rate reading
to the alarm and react incorrectly.

## Example Description

This example illustrates how to use the PresentationQosPolicy coherent_access,
GROUP presentation, and ordered_access to force writes across multiple writers
belonging to the same publisher to be grouped and ordered on the receiving side.

All samples sent between `begin_coherent_changes` and `end_coherent_changes`
will be available before the readers are notified that there are samples to
read. Because we are waiting on samples for multiple readers, we wait for the 
subscriber to notify us that at least one of its readers has data to read using
the DATA_ON_READERS status (as opposed to waiting for the DATA_AVAILABLE status
of each reader individually). At that time, we can lookup the list of readers
with data pending to be read and take the samples for the coherent set in order.

In this example, the subscriber receives updates on two topics: Checkpoint Time
and Checkpoint Place. Because we request coherent access at the group level,
on_data_on_readers is not triggered until the publisher ends the coherent
changes, so we get a set of coherent samples rather than being notified
as each sample arrives.

Note that coherency alone does *not* guarantee that observers will see updates
atomically, only that all samples will be available. For instance, if another
thread interrupts the DDS receiver thread after on_data_on_readers has processed
only four updates, the interrupting thread will see an inconsistent state for
the objects.

Coherent presentation currently requires reliable communication. Additionally,
the reader history depth must be sufficiently large to store all samples
received during the coherent change group.
