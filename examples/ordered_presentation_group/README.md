# Example Code: Ordered Presentation Group

## Concept
Usually *DataReaders* will receive data in the order that it was sent by a
*DataWriter*. In addition, data is presented to the *DataReader* as soon as the
application receives the next value expected.

Sometimes, you may want a set of data for the same *DataWriter* to be presented
to the receiving *DataReader* only after **ALL** the elements of the set have
been received, but not before. You may also want the data to be presented in a
different order than it was received. Specifically, for keyed data, you may want
*Connext* to present the data in keyed or instance order.

If we use *Ordered Presentation*, the *access_scope* controls the scope of the
order in which samples are presented to the subscribing application. The
*access_scope* may have four different values:

- If *access_scope* is `INSTANCE`, the relative order of samples sent by a
  *DataWriter* is only preserved on an per-instance basis. If two samples
  refer to the same instance (identified by Topic and a particular value for
  the key) then the order in which they are stored in the *DataReader’s* queue
  is consistent with the order in which the changes occurred. However, if the
  two samples belong to different instances, the order in which they are
  presented may or may not match the order in which the changes occurred.

- If *access_scope* is `TOPIC`, the relative order of samples sent by a
  *DataWriter* is preserved for all samples of all instances. The coherent
  grouping and/or order in which samples appear in the *DataReader’s* queue is
  consistent with the grouping/order in which the changes occurred—even if
  the samples affect different instances.

- If *access_scope* is `GROUP`, the scope spans all instances belonging to
  *DataWriter* entities within the same *Publisher*—even if they are instances
  of different topics. Changes made to instances via *DataWriter* entities
  attached to the same *Publisher* are made available to *Subscribers* on the
  same order they occurred.

- If *access_scope* is `HIGHEST_OFFERED`, the Subscriber will use the access
  scope specified by each remote *Publisher*.

## Example Description
In this example we illustrate how to use `GROUP` *access_scope* for the order in
which samples are presented to the subscribing application.

The Publisher sets its presentation QoS properties to enable GROUP-level
ordered access. This enforces ordering on instances from any *DataWriters* and
*Topics* for a given *Publisher*. Also note that ordered-access configuration
needs to be configured in the subscription side so samples are read in an
ordered manner.

The subscriber application illustrates the effects of the `GROUP` *access_scope*
presentation QoS. Changes made to instances via *DataWriter* entities attached
to the same *Publisher* are made available to *Subscribers* on the same order
they occurred. For more information about these QoS you can see section
"PRESENTATION QosPolicy" of the User's Manual.

The example creates 3 Topics, 3 DataWriters, 3 DataReaders and 3 instances.
Each *DataWriter* publishes two samples of the corresponding topic. This process
is repeated once a second. The order in which the DataWriters publish the
samples is: `{ DW1-Sample1, DW1-Sample2, DW2-Sample1, DW2-Sample2, DW3-Sample1,
DW3-Sample2 }`.

The key points in the example are:

- In the Subscriber, we implement a subscriber listener for the callback
`ON_DATA_ON_READERS`. This callback will activate every time any DataReader has
data available.

We want to read the samples in the order in which they were modified. For doing
that, in the implementation of the callback we invoke `begin_access()` and
`end_access()`. Inside this block, we call `get_datareaders()` to obtain an
ordered sequence of the *DataReaders*. This sequence specifies the reading order
for the samples. See more information in sections "Beginning and Ending
Group-Ordered Access" and "Getting DataReaders with Specific Samples" of
the User's Manual.

We need to iterate across the sequence of *DataReaders* and read one sample each
time. For doing this, use `take_next_sample()` instead of `take()`.

- In *USER_QOS_PROFILES.xml*, we set Group Access Scope and Ordered Access QoS:

```xml
<publisher_qos>
    <presentation>
        <ordered_access>true</ordered_access>
        <access_scope>GROUP_PRESENTATION_QOS</access_scope>
    </presentation>
</publisher_qos>

<subscriber_qos>
    <presentation>
        <ordered_access>true</ordered_access>
        <access_scope>GROUP_PRESENTATION_QOS</access_scope>
    </presentation>
</subscriber_qos>
```
