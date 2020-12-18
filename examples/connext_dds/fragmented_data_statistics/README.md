# Example Code: Fragmented Data Statistics

Example about getting fragmented data statistics.

## Concept

RTI Connext DDS provides information about fragmented data statistics.

| Structure | Field Name | Description |
| --------- | ---------- | ----------- |
| DDS_DataWriterProtocolStatus | pushed_sample_count | The number of user samples pushed on write from a local DataWriter to a matching remote DataReader. |
| DDS_DataWriterProtocolStatus | pulled_sample_count | The number of user samples pulled from local DataWriter by matching DataReaders. |
| DDS_DataWriterProtocolStatus | pushed_fragment_count | The number of DATA_FRAG messages that have been pushed by this DataWriter. |
| DDS_DataWriterProtocolStatus | pulled_fragment_count | The number of DATA_FRAG messages that have been pulled from this DataWriter. |
| DataReaderProtocolStatus | received_fragment_count | The number of DATA_FRAG messages that have been received by this DataReader. |
| DataReaderProtocolStatus | dropped_fragment_count | The number of DATA_FRAG messages that have been dropped by a DataReader. |
| DataReaderProtocolStatus | sent_nack_fragment_count | The number of NACK fragments that have been sent from a DataReader to a DataWriter. |
| DataReaderProtocolStatus | sent_nack_fragment_bytes | The number of NACK fragment bytes that have been sent from a DataReader to a DataWriter. |

For more information about the counters, please refer to the API Reference HTML
documentation or the User Manuals.

## Example Description

In this example, we illustrate how to access the fragmented data statistics:

In order to update the statistics we have written large data samples, which is
fragmented.

The configuration we are using results in each sample being made up of 3
fragments. Assuming no repairs are sent, this means that the fragment counters
should be three times higher than the sample counters.

On the subscriber application we expect:

```plaintext
Fragmented Data Statistics:
     received_fragment_count 330
     dropped_fragment_count 0
     sent_nack_fragment_count 0
     sent_nack_fragment_bytes 0
```

On the publisher application we expect:

```plaintext
Fragmented Data Statistics:
     pushed_fragment_count 420
     pushed_fragment_bytes 1136800
     pulled_fragment_count 0
     pulled_fragment_bytes 0
     received_nack_fragment_count 0
     received_nack_fragment_bytes 0
```

In the example we are viewing the statistics by obtain the DataReader and
DataWriter protocol statuses. These statistics are also published by the
monitoring libraries, and thus can be viewed through RTI monitor.
