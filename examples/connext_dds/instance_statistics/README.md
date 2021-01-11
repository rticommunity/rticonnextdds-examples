# Example Code: Instance Statistics

Example showing how to obtain instance statistics.

## Concept

RTI Connext DDS provides information about the instances that are currently
being maintained by a DataReader or DataWriter.

| Structure | Field Name | Description |
| --------- | ---------- | ----------- |
| DataReaderCacheStatus | alive_instance_count | The number of instances in the DataReader's queue with an instance state equal to DDS_ALIVE_INSTANCE_STATE. |
| DataReaderCacheStatus | no_writers_instance_count | The number of instances in the DataReader's queue with an instance state equal to DDS_NOT_ALIVE_NO_WRITERS_INSTANCE_STATE. |
| DataReaderCacheStatus | disposed_instance_count | The number of instances in the DataReader's queue with an instance state equal to DDS_NOT_ALIVE_DISPOSED_INSTANCE_STATE. |
| DataWriterCacheStatus | alive_instance_count | The number of instances currently in the DataWriter's queue that have an instance_state equal to DDS_ALIVE_INSTANCE_STATE. |
| DataWriterCacheStatus | disposed_instance_count | The number of instances currently in the DataWriter's queue that have an instance_state equal to DDS_NOT_ALIVE_DISPOSED_INSTANCE_STATE (due to, for example, being disposed via the FooDataWriter_dispose operation). |
| DataWriterCacheStatus | unregistered_instance_count | The number of instances currently in the DataWriter's queue that the DataWriter has unregistered from via the FooDataWriter_unregister_instance operation. |

For more information about the counters, please refer to the API Reference HTML
documentation or the [User Manuals](https://community.rti.com/static/documentation/connext-dds/6.1.0/doc/manuals/connext_dds_professional/users_manual/index.htm#users_manual/Statuses_for_DataReaders.htm).

## Example Description

In this example, we illustrate how to access the instance statistics.

In order to update the instance statistics we write, dispose and unregister
different instances from a DataWriter.

In the subscriber application, we retrieve the DataReader Cache Status and log
the relevant values. The expected output is:

```plaintext
Instance statistics:
     alive_instance_count 16
     no_writers_instance_count 0
     detached_instance_count 22
     disposed_instance_count 7
```

In the publisher application, we retrieve the DataWriter Cache Status and log
the relevant values. The expected output is:

```plaintext
Instance statistics:
     alive_instance_count 15
     unregistered_instance_count 1
     disposed_instance_count 7
```

In the example we are viewing the statistics by obtaining the DataReader and
DataWriter cache statuses. These statistics are also published by the
monitoring libraries, and thus can be viewed through RTI monitor.
