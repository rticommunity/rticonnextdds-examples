# Example Code: Detect Samples Dropped by the DataReader

Example about detecting samples dropped.

## Concept

RTI Connext DDS provides information through statistics about every sample that
is not seen by a subscribing application.
Dropped samples are exposed through counters in the DataReaderCacheStatus and
DataReaderProtocolStatus:

| Structure | Field Name | Description |
| --------- | ---------- | ----------- |
| DataReaderCacheStatus| old_source_timestamp_dropped_sample_count | The number of samples dropped as a result of receiving a sample older than the last one, using DDS_BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS.|
| DataReaderCacheStatus| tolerance_source_timestamp_dropped_sample_count |The number of samples dropped as a result of receiving a sample in the future, using DDS_BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS. |
| DataReaderCacheStatus| ownership_dropped_sample_count | The number of samples dropped as a result of receiving a sample from a DataWriter with a lower strength, using Exclusive Ownership. |
| DataReaderCacheStatus| content_filter_dropped_sample_count | The number of user samples filtered by the DataReader due to Content-Filtered Topics. |
| DataReaderCacheStatus| time_based_filter_dropped_sample_count | The number of user samples filtered by the DataReader due to DDS_TimeBasedFilterQosPolicy. |
| DataReaderCacheStatus| expired_dropped_sample_count | The number of samples expired by the DataReader due to DDS_LifespanQosPolicy or the autopurge sample delays. |
| DataReaderCacheStatus| virtual_duplicate_dropped_sample_count | The number of virtual duplicate samples dropped by the DataReader. A sample is a virtual duplicate if it has the same identity (Virtual Writer GUID and Virtual Sequence Number) as a previously received sample. |
| DataReaderCacheStatus| replaced_dropped_sample_count | The number of samples replaced by the DataReader due to DDS_KEEP_LAST_HISTORY_QOS replacement. |
| DataReaderCacheStatus| writer_removed_batch_sample_dropped_sample_count | The number of batch samples received by the DataReader that were marked as removed by the DataWriter. |
| DataReaderProtocolStatus| out_of_range_rejected_sample_count | The number of samples dropped by the DataReader due to the receive window being full and the sample received out-of-order. |

For more information about the counters, please refer to the API Reference HTML
documentation or the [User Manuals](https://community.rti.com/static/documentation/connext-dds/6.1.0/doc/manuals/connext_dds_professional/users_manual/index.htm#users_manual/Statuses_for_DataReaders.htm).

## Example Description

In this example, we illustrate how to detect samples dropped for two reasons:

- Exclusive Ownership.
- Content Filter Topic.

In order to force the samples to be dropped we:

-   In the publisher side:

    -   Create two DataWriters with Exclusive Ownership and different values for
    the Ownership Strength.
    -   Both DataWriters write samples using different values for the element X,
    which we will filter out.
-   In the subscriber side:

    - Create a Content Filter Topic, filtering out specific values of element X.
    - Create a DataReader enabling Exclusive Ownership.

In the subscriber application, we retrieve the DataReader Cache Status and log
the relevant values. The expected output is:

```plaintext
Samples dropped:
     ownership_dropped_sample_count 4
     content_filter_dropped_sample_count 3
```

The above output informs us that four samples were dropped due to Exclusive
Ownership and three samples dropped due to Content Filter Topic.

In the example we are viewing the statistics by obtaining the DataReader cache
status. These statistics are also published by the monitoring libraries, and
thus can be viewed through RTI monitor.
