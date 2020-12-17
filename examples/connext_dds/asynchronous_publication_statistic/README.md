# Example Code: Print QoS

Example about getting asynchornous publisher statistic.

## Concept

RTI Connext DDS provides information about asynchronous publisher statistic data.

| Structure | Field Name | Description |
| --------- | ---------- | ----------- |
| DDS_DataWriterProtocolStatus | pushed_sample_count | The number of user samples pushed on write from a local DataWriter to a matching remote DataReader. |
| DDS_DataWriterProtocolStatus | pulled_sample_count | The number of user samples pulled from local DataWriter by matching DataReaders. |
| DDS_DataWriterProtocolStatus | pushed_fragment_count | The number of DATA_FRAG messages that have been pushed by this DataWriter. |
| DDS_DataWriterProtocolStatus | pulled_fragment_count | The number of DATA_FRAG messages that have been pulled from this DataWriter. |
| DataReaderProtocolStatus | received_sample_count | The number of samples received by a DataReader. |
| DataReaderProtocolStatus | received_fragment_count | The number of DATA_FRAG messages that have been received by this DataReader. |

For more information about the counters, please refer to the API Reference HTML
documentation or the User Manuals.

## Example Description

In this example, we illustrate how to access the asynchronous publisher statistic:

In order to update the instance statistic we have write several sample using
asynchronous publisher.

The output of the subscriber application will look like:

```
Asynchronous Publisher Statistics:
     received_sample_count 25
     received_fragment_count 75
```

The output of the publisher application will look like:

```
Asynchronous Publisher Statistics:
     pushed_sample_count 44
     pulled_sample_count 0
     pushed_fragment_count 132
     pulled_fragment_count 0
```