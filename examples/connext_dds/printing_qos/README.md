# Example Code: Printing QoS

Example showing how to print the QoS in use by DDS Entities.

## Concept

RTI Connext DDS provides the possibility to print the following QoS objects:
`DataReaderQos`, `DataWriterQos`, `PublisherQos`, `SubscriberQos`, `TopicQos`,
`DomainParticipantQos` and `DomainParticipantFactoryQos`.

-   In C, there are three new APIs per top-level QoS object (DataWriterQos is
 used as an example below):

```plaintext
DDS_DataWriterQos_print(const struct DDS_DataWriterQos *self)
DDS_DataWriterQos_to_string(const struct DDS_DataWriterQos *self, char *string, DDS_UnsignedLong *string_size)
DDS_DataWriterQos_to_string_w_params(const struct DDS_DataWriterQos *self, char *string, DDS_UnsignedLong *string_size, const struct DDS_DataWriterQos *base, const struct DDS_QosPrintFormat *format)
```

- In Traditional C++, the same functionality is achieved through overloads:

```plaintext
DDS_DataWriterQos::print()
DDS_DataWriterQos::to_string(char *string, DDS_UnsignedLong& string_size)
DDS_DataWriterQos::to_string(char *string, DDS_UnsignedLong& string_size, const DDS_DataWriterQos& base)
DDS_DataWriterQos::to_string(char *string, DDS_UnsignedLong& string_size, const DDS_QosPrintFormat &format)
DDS_DataWriterQos::to_string(char *string, DDS_UnsignedLong& string_size, const DDS_DataWriterQos &format, const DDS_QosPrintFormat &format)
```

- In Modern C++, the to_string APIs are free-standing functions:

```plaintext
std::string to_string(const DataWriterQos& qos, const QosPrintFormat& format = QosPrintFormat())
std::string to_string(const DataWriterQos& qos, const DataWriterQos& base, const QosPrintFormat& format = QosPrintFormat())
std::string to_string(const DataWriterQos& qos, const qos_print_all_t& qos_print_all, const QosPrintFormat& format = QosPrintFormat())
std::ostream& operator<<(std::ostream& out, const DataWriterQos& qos)
```

- In Java, Object.toString is overridden, and additional overloads are available:

```plaintext
String DataWriterQos.toString()
String DataWriterQos.toString(DataWriterQos baseQos, QosPrintFormat format)
String DataWriterQos.toString(QosPrintFormat format)
String DataWriterQos.toString(DataWriterQos baseQos)
```

- In .NET, Object.ToString is overridden, and additional overloads are available:

```plaintext
String ^DataWriterQos::ToString()
String ^DataWriterQos::ToString(DataWriterQos ^base, QosPrintFormat ^format)
String ^DataWriterQos::ToString(QosPrintFormat ^format)
String ^DataWriterQos::ToString(DataWriterQos ^base)
```

For more information about the use of these APIs, please refer to the API
Reference HTML documentation or the [User Manuals](https://community.rti.com/static/documentation/connext-dds/6.1.0/doc/manuals/connext_dds_professional/users_manual/index.htm#users_manual/QoS_Profile_Inheritance.htm#19.3.3.5_Viewing_Resolved_QoS_Values).

In addition to new APIs, when logging verbosity is set to at least LOCAL, and
category is set to API, the QoS configuration being used by an entity is logged
when the entity is created or when the QoS is updated (via the set_qos
operation).

## Example Description

RTI Connext DDS provides you the possibility to print the following Qos objects:
`DataReaderQos`, `DataWriterQos`, `PublisherQos`, `SubscriberQos`, `TopicQos`,
`DomainParticipantQos` and `DomainParticipantFactoryQos`. It is possible to log
all the XML or just the differences with regards to the documented defaults.

In this example, we illustrate how to use the printing QoS APIs:

-   In the `printing_publisher` file we demostrate how to use:
    - `DDS_PublisherQos_to_string_w_params`
    - `DDS_TopicQos_to_string`
    - `DDS_DataWriterQos_print`
-   In the `printing_subscriber` file we demostrate how to use:
    - `DDS_DomainParticipantQos_to_string`
    - `DDS_SubscriberQos_to_string_w_params`
    - `DDS_DataReaderQos_print`

For example the output of the PublisherQos will be:

```xml
<publisher_qos>
    <presentation>
        <access_scope>INSTANCE_PRESENTATION_QOS</access_scope>
        <coherent_access>false</coherent_access>
        <ordered_access>false</ordered_access>
        <drop_incomplete_coherent_set>true</drop_incomplete_coherent_set>
    </presentation>
    <partition>
        <name>
        </name>
    </partition>
    <group_data>
    </group_data>
    <entity_factory>
        <autoenable_created_entities>true</autoenable_created_entities>
    </entity_factory>
    <asynchronous_publisher>
        <disable_asynchronous_write>false</disable_asynchronous_write>
        <thread>
            <mask></mask>
            <priority>-9999999</priority>
            <stack_size>-1</stack_size>
            <cpu_list>
            </cpu_list>
            <cpu_rotation>THREAD_SETTINGS_CPU_NO_ROTATION</cpu_rotation>
        </thread>
        <disable_asynchronous_batch>false</disable_asynchronous_batch>
        <asynchronous_batch_thread>
            <mask></mask>
            <priority>-9999999</priority>
            <stack_size>-1</stack_size>
            <cpu_list>
            </cpu_list>
            <cpu_rotation>THREAD_SETTINGS_CPU_NO_ROTATION</cpu_rotation>
        </asynchronous_batch_thread>
        <asynchronous_batch_blocking_kind>DDS_SEMAPHORE_BLOCKING_KIND</asynchronous_batch_blocking_kind>
        <disable_topic_query_publication>false</disable_topic_query_publication>
        <topic_query_publication_thread>
            <mask></mask>
            <priority>-9999999</priority>
            <stack_size>-1</stack_size>
            <cpu_list>
            </cpu_list>
            <cpu_rotation>THREAD_SETTINGS_CPU_NO_ROTATION</cpu_rotation>
        </topic_query_publication_thread>
    </asynchronous_publisher>
    <exclusive_area>
        <use_shared_exclusive_area>false</use_shared_exclusive_area>
    </exclusive_area>
    <publisher_name>
    </publisher_name>
</publisher_qos>
```
