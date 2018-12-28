# Example Code: Built-in QoS Profiles

## Concept
*RTI Connext DDS* provides *Quality of Service* (QoS) that controls the behavior
of the different DDS entities, and allows you to configure the *middleware* to
enable features such as monitoring. To make this process easier, RTI includes
a set of built-in QoS profiles that provide useful functionality, such as
enabling monitoring or configuring *DataWriters* and *DataReaders* to be
strictly reliable.

The full list of built-in profiles is here:

* In library `BuiltinQosLib`:
  * Baseline
  * Baseline.5.0.0
  * Baseline.5.1.0
  * Baseline 5.2.0
  * Generic.Common
  * Generic.Monitoring.Common
  * Generic.ConnextMicroCompatibility
  * Generic.OtherDDSVendorCompatibility
  * Generic.510TransportCompatibility


* In library for experimental profiles (`BuiltinQosLibExp`):
  * Generic.StrictReliable
  * Generic.KeepLastReliable
  * Generic.BestEffort
  * Generic.StrictReliable.HighThroughput
  * Generic.StrictReliable.LowLatency
  * Generic.Participant.LargeData
  * Generic.Participant.LargeData.Monitoring
  * Generic.StrictReliable.LargeData
  * Generic.KeepLastReliable.LargeData
  * Generic.StrictReliable.LargeData.FastFlow
  * Generic.StrictReliable.LargeData.MediumFlow
  * Generic.StrictReliable.LargeData.SlowFlow
  * Generic.KeepLastReliable.LargeData.FastFlow
  * Generic.KeepLastReliable.LargeData.MediumFlow
  * Generic.KeepLastReliable.LargeData.SlowFlow
  * Generic.KeepLastReliable.TransientLocal
  * Generic.KeepLastReliable.Transient
  * Generic.KeepLastReliable.Persistent
  * Generic.AutoTuning
  * Pattern.PeriodicData
  * Pattern.Streaming
  * Pattern.ReliableStreaming
  * Pattern.Event
  * Pattern.AlarmEvent
  * Pattern.Status
  * Pattern.AlarmStatus
  * Pattern.LastValueCache


## Example Description
This example shows how to use built-in profiles in both the XML QoS file and
in the source code. In the XML, the default QoS profiles described in the
file inherit from two different built-in profiles.

1. The overall default profile inherits from the
`BuiltinQoSLibExp::Pattern.ReliableStreaming` profile:
```xml
<qos_profile name="profiles_Profile" base_name="BuiltinQosLibExp::Pattern.ReliableStreaming" is_default_qos="true">
```

2. The *DomainParticipant's* QoS overrides this so that the *DomainParticipant*
can inherit directly from a profile that enables monitoring. This allows the
*DomainParticipant* to take its specific behavior from a different profile than
the *DataWriter* and *DataReader*, since the *DomainParticipant's* QoS is
independent of the reader, and writer-specific streaming reliability QoS:
```xml
<participant_qos base_name="BuiltinQosLib::Generic.Monitoring.Common">
```

In this example, the *DomainParticipant* is configured to enable monitoring out
of the box.

**Note that this will require that you link against RTI's dynamic libraries** so
the monitoring library can be loaded correctly. See the README file within each
directory for more details on linking dynamically.

The *DataWriter* and *DataReader* are configured with a QoS profile to enable
streaming reliable data.
