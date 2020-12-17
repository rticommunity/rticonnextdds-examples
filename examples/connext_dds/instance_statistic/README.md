# Example Code: Print QoS

Example about getting instance statistic.

## Concept

RTI Connext DDS provides information about the instances that are currently
being maintained by that DataReader or DataWriter.

| Structure | Field Name | Description |
| --------- | ---------- | ----------- |
| DataReaderCacheStatus | alive_instance_count | The number of instances in the DataReader's queue with an instance state equal to DDS_ALIVE_INSTANCE_STATE. |
| DataReaderCacheStatus | no_writers_instance_count | The number of instances in the DataReader's queue with an instance state equal to DDS_NOT_ALIVE_NO_WRITERS_INSTANCE_STATE. |
| DataReaderCacheStatus | disposed_instance_count | The number of instances in the DataReader's queue with an instance state equal to DDS_NOT_ALIVE_DISPOSED_INSTANCE_STATE. |
| DataWriterCacheStatus | alive_instance_count | The number of instances currently in the DataWriter's queue that have an instance_state equal to DDS_ALIVE_INSTANCE_STATE. |
| DataWriterCacheStatus | disposed_instance_count | The number of instances currently in the DataWriter's queue that have an instance_state equal to DDS_NOT_ALIVE_DISPOSED_INSTANCE_STATE (due to, for example, being disposed via the FooDataWriter_dispose operation). |
| DataWriterCacheStatus | unregistered_instance_count | The number of instances currently in the DataWriter's queue that the DataWriter has unregistered from via the FooDataWriter_unregister_instance operation. |

For more information about the counters, please refer to the API Reference HTML
documentation or the User Manuals.

## Example Description

In this example, we illustrate how to access the instance statistic:

In order to update the instance statistic we have write, dispose and unregister
different instances.

The output of the subscriber application will look like:

```
Instance statistic:
     alive_instance_count 16
     no_writers_instance_count 0
     detached_instance_count 22
     disposed_instance_count 7
```

The output of the publisher application will look like:

```
Instance statistic:
     alive_instance_count 15
     unregistered_instance_count 1
     disposed_instance_count 7
```
