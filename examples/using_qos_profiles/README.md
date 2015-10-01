# Example Code: Using QoS Profiles

## Concept
A QoS profile is a collection of QoS settings, usually one per entity, specified
in XML format. For example:
```xml
<qos_profile name="StrictReliableCommunicationProfile">
   <datawriter_qos>
      <history>
         <kind>KEEP_ALL_HISTORY_QOS</kind>
      </history>
      <reliability>
         <kind>RELIABLE_RELIABILITY_QOS</kind>
      </reliability>
   </datawriter_qos>
   <datareader_qos>
      <history>
         <kind>KEEP_ALL_HISTORY_QOS</kind>
      </history>
      <reliability>
         <kind>RELIABLE_RELIABILITY_QOS</kind>
      </reliability>
   </datareader_qos>
</qos_profile>
```

By using QoS profiles, you can change QoS settings without recompiling the
application.

## Example description
In this example, we create a publisher and a subscriber application that
communicate using two different QoS profiles: *volatile_profile* and
*transient_local_profile*.

In the publisher application we create a *DomainParticipant* and a *Publisher*.
With this *Publisher*, we create two *DataWriters* that use *volatile_profile*
and *transient_local_profile* QoS, respectively. Accordingly, in the subscriber
application, we create a *DomainParticipant* and a *Subscriber*, from which
we create two *DataReaders* that use *volatile_profile* and
*transient_local_profile* QoS, respectively.

We have defined these two QoS profiles in two different files,
*USER_QOS_PROFILES.xml* and *my_custom_qos_profiles.xml*, to illustrate
some of the different ways in which the *DomainParticipantFactory* may load
XML-specified QoS into your application.

* *USER_QOS_PROFILES.xml* includes the definition of *volatile_profile*, which
  enables `VOLATILE_DURABILITY` QoS for both *DataWriter* and *DataReader*.
  The *DomainParticipantFactory* loads this file automatically if it exists in
  the working directoy, i.e., if we run the application from the directory where
  *USER_QOS_PROFILES.xml* is. In the definition of *volatile_profile*, we set
  `is_default_qos="true"` to enable its policies as the default QoS. As a
  result, when we create an entity using `DDS_<ENTITY_NAME>_QOS_DEFAULT` in our
  example, we implicitly using the *volatile_profile*.

* *my_custom_qos_profiles.xml* includes the definition of
  *transient_local_profile*, which enables `TRANSIENT_LOCAL_DURABILITY` QoS for
  both *DataWriter* and *DataReader*. We also include a profile called
  *transient_profile*. Unlike *USER_QOS_PROFILES.xml*, the XML-specified QoSs
  defined in *my_custom_qos_profiles.xml* are not loaded automatically by the
  *DomainParticipantFactory*. We need to modify the *DDSTheParticipantFactory
  Profile* QoSPolicy so it can find the QoS policies defined in different XML
  files.

``` c
    factory_qos.profile.url_profile[0] =
            DDS_String_dup("my_custom_qos_profiles.xml");
```

Note that *url_profile* is a sequence, so in the example above we are only
adding an XML file to the first element of the sequence.

If you run the example you will see that the *DataReader* using
*volatile_profile* reads the examples published by both volatile and
transient local *DataWriters*, whereas the transient local *DataReader* reads
only the samples published by the transient local *DataWriter*. The reason
behind this behavior the value offered is considered compatible with the value
requested if and only if the inequality offered kind >= requested kind evaluates
to 'TRUE'. For the purposes of this inequality, the values of `DURABILITY` kind
are considered ordered such that `VOLATILE_DURABILITY_QOS <
TRANSIENT_LOCAL_DURABILITY_QOS < TRANSIENT_DURABILITY_QOS <
PERSISTENT_DURABILITY_QOS`.
