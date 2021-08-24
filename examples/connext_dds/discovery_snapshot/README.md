# Example Code: Discovery Snapshot

Example showing how to debug connection issues taking discovery snapshots.

## Concept

RTI Connext DDS allows you to take discovery snapshots of DomainParticipants, DataWriters,
and DataReaders. Discovery snapshots are useful when your applications are not communicating as expected.

-   In C, there are three new APIs, one per entity (file_name can be NULL to print through RTI Connext Logging):

```plaintext
DDS_ReturnCode_t DDS_DomainParticipant_take_discovery_snapshot(DDS_DomainParticipant *self, const char *file_name)
DDS_ReturnCode_t DDS_DataWriter_take_discovery_snapshot(DDS_DataWriter *self, const char *file_name)
DDS_ReturnCode_t DDS_DataReader_take_discovery_snapshot(DDS_DataReader *self, const char *file_name)
```

-   In Traditional C++, the same functionality exists as a member function of each entity:

```plaintext
DDS_ReturnCode_t DDSDomainParticipant::take_discovery_snapshot()
DDS_ReturnCode_t DDSDomainParticipant::take_discovery_snapshot(const char *file_name)
DDS_ReturnCode_t DDSDataWriter::take_discovery_snapshot()
DDS_ReturnCode_t DDSDataWriter::take_discovery_snapshot(const char *file_name)
DDS_ReturnCode_t DDSDataReader::take_discovery_snapshot()
DDS_ReturnCode_t DDSDataReader::take_discovery_snapshot(const char *file_name)
```

-   In Modern C++, the APIs are standalone functions:

```plaintext
void take_snapshot(dds::domain::DomainParticipant participant)
void take_snapshot(dds::domain::DomainParticipant participant, const std::string& file_name)
void take_snapshot(dds::pub::DataWriter writer)
void take_snapshot(dds::pub::DataWriter writer, const std::string& file_name)
void take_snapshot(dds::sub::DataReader reader)
void take_snapshot(dds::sub::DataReader reader, const std::string& file_name)
```

-   In Java, this functionality is present as a member function:

```plaintext
void com.rti.dds.domain.DomainParticipant.take_discovery_snapshot()
void com.rti.dds.domain.DomainParticipant.take_discovery_snapshot(String file_name)
void com.rti.dds.publication.DataWriter.take_discovery_snapshot()
void com.rti.dds.publication.DataWriter.take_discovery_snapshot(String file_name)
void com.rti.dds.subscription.DataReader.take_discovery_snapshot()
void com.rti.dds.subscription.DataReader.take_discovery_snapshot(String file_name)
```

-   In .NET/C#, the APIs are part of the class DiscoverySnapshot:

```plaintext
void DiscoverySnapshot.TakeSnapshot(DomainParticipant participant)
void DiscoverySnapshot.TakeSnapshot(DomainParticipant participant, string fileName)
void DiscoverySnapshot.TakeSnapshot(DataWriter writer)
void DiscoverySnapshot.TakeSnapshot(DataWriter writer, string fileName)
void DiscoverySnapshot.TakeSnapshot(DataReader reader)
void DiscoverySnapshot.TakeSnapshot(DataReader reader, string fileName)
```

For more information about the use of these APIs, please refer to the API
Reference HTML documentation or the [User Manuals]() (not yet available).

The output information can be printed to a file (if you provide a file name to the API) or through
RTI Connext Logging (if you do not provide a file name to the API).

## Example Description

In this example, we illustrate how to use the discovery snapshot APIs in the `discovery_snapshot_publisher`:

-   Taking the snapshot for the participant.
-   Taking the snapshot for the data writer.

At the beginning, writer and reader will match, but they will not be compatible.

The output of the participant snapshot will be:

```plaintext
----------------------------------------------------------------------------
Remote participants that match the local participant domain=0 name="discovery_snapshotParticipant"
role="discovery_snapshotParticipantRole" id="1" guid_prefix="0x0101E315,0x5357026F,0x42882524" 
----------------------------------------------------------------------------
1. 0x0101A343,0x260898AB,0xDEE35DB5 name="discovery_snapshotParticipant" role="discovery_snapshotParticipantRole"
unicastLocators="udpv4://192.168.1.36:7411,udpv4://10.2.190.25:7411,shmem://C268:F783:F9C1:8665:B8B1:B40F:0000:0000:7411" 
----------------------------------------------------------------------------
```

This means that the participant in the reader side are discovered by the participant in the writer side.

The output of the data writer snapshot will be:

```plaintext
----------------------------------------------------------------------------
Remote readers that match the local writer domain=0 name="discovery_snapshotDataWriter"
guid="0x0101E315,0x5357026F,0x42882524:0x80000003" topic="Example DiscoverySnapshot" type="DiscoverySnapshot" 
----------------------------------------------------------------------------
Compatible readers:
Incompatible readers:
1. 0x0101A343,0x260898AB,0xDEE35DB5:0x80000004 name="discovery_snapshotDataReader"
kind="unkeyed user datareader" unicastLocators="udpv4://192.168.1.36:7411,udpv4://10.2.190.25:7411,
shmem://C268:F783:F9C1:8665:B8B1:B40F:0000:0000:7411" reason="Inconsistent QoS" 
```

This means that writer and reader match each other (because the reader appears in the writer discovery snapshot)
but they are not compatible (because the reader appears like an incompatible reader). The field `reason` says
`Inconsistent QoS`, so now we know why writer and reader are incompatible. Let's take a look at
`USER_QOS_PROFILE.xml`. We can see the following lines in the data reader QoS:

```xml
<ownership>
    <kind>DDS_EXCLUSIVE_OWNERSHIP_QOS</kind>
</ownership>
```

The value of ownership QoS must be set to the same kind on both the publishing and subscribing sides.
The default value for this QoS is `DDS_SHARED_OWNERSHIP_QOS`, so in this example there is
a data writer with exclusive ownership and a data reader with shared ownership. If you delete or
comment these lines, you will fix the issue. For more information about the ownership QoS,
please refer to the [User Manuals](https://community.rti.com/static/documentation/connext-dds/6.1.0/doc/manuals/connext_dds_professional/users_manual/index.htm#users_manual/OWNERSHIP_QosPolicy.htm#7.5.17_OWNERSHIP_QosPolicy%3FTocPath%3DPart%25202%253A%2520Core%2520Concepts%7C7.%2520Sending%2520Data%7C7.5%2520DataWriter%2520QosPolicies%7C7.5.17%2520OWNERSHIP%2520QosPolicy%7C_____0).

After deleting these lines and retry, writer and reader will communicate and you will obtain
the following discovery snapshot, that shows that both writer and reader are compatible:

```plaintext
----------------------------------------------------------------------------
Remote readers that match the local writer domain=0 name="discovery_snapshotDataWriter" 
guid="0x01011EA8,0xA5946F68,0x476E6EE4:0x80000003" topic="Example DiscoverySnapshot" type="DiscoverySnapshot" 
----------------------------------------------------------------------------
Compatible readers:
1. 0x01019F51,0x23EFD9CC,0xA11429BC:0x80000004 name="discovery_snapshotDataReader"
kind="unkeyed user datareader" unicastLocators="udpv4://192.168.1.36:7411,udpv4://10.2.190.25:7411,
shmem://C268:F783:F9C1:8665:B8B1:B40F:0000:0000:7411" liveliness="ALIVE" 
Incompatible readers: 
```