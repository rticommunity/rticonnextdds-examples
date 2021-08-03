/*******************************************************************************
 (c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/

import java.util.Objects;

import com.rti.dds.domain.DomainParticipant;
import com.rti.dds.domain.DomainParticipantFactory;
import com.rti.dds.infrastructure.InstanceHandle_t;
import com.rti.dds.infrastructure.StatusKind;
import com.rti.dds.publication.Publisher;
import com.rti.dds.topic.Topic;

// ===========================================================================

public class cftPublisher extends Application implements AutoCloseable {
    // Usually one per application
    private DomainParticipant participant = null;

    private void runApplication()
    {
        // Start communicating in a domain
        participant = Objects.requireNonNull(
                DomainParticipantFactory.get_instance().create_participant(
                        getDomainId(),
                        DomainParticipantFactory.PARTICIPANT_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        // A Publisher allows an application to create one or more DataWriters
        Publisher publisher =
                Objects.requireNonNull(participant.create_publisher(
                        DomainParticipant.PUBLISHER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        // Register the datatype to use when creating the Topic
        String typeName = cftTypeSupport.get_type_name();
        cftTypeSupport.register_type(participant, typeName);

        // Create a Topic with a name and a datatype
        Topic topic = Objects.requireNonNull(participant.create_topic(
                "Example cft",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));

        // This DataWriter writes data on "Example cft" Topic
        cftDataWriter writer = (cftDataWriter) Objects.requireNonNull(
                publisher.create_datawriter(
                        topic,
                        Publisher.DATAWRITER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        /* If you want to set the reliability and history QoS settings
         * programmatically rather than using the XML, you will need to add
         * the following lines to your code and comment out the
         * create_datawriter call above.
         */

        /*
            DataWriterQos datawriter_qos = new DataWriterQos();
            publisher.get_default_datawriter_qos(datawriter_qos);

            datawriter_qos.reliability.kind =
                ReliabilityQosPolicyKind.RELIABLE_RELIABILITY_QOS;
            datawriter_qos.durability.kind =
                DurabilityQosPolicyKind.TRANSIENT_LOCAL_DURABILITY_QOS;
            datawriter_qos.history.kind =
                HistoryQosPolicyKind.KEEP_LAST_HISTORY_QOS;
            datawriter_qos.history.depth = 20;

            writer = (cftDataWriter) Objects.requireNonNull(
                    publisher.create_datawriter(
                        topic, datawriter_qos,
                        null, StatusKind.STATUS_MASK_NONE));
        */

        // Create data sample for writing
        cft data = new cft();

        InstanceHandle_t instance_handle = InstanceHandle_t.HANDLE_NIL;
        /* For a data type that has a key, if the same instance is going to
         * be written multiple times, initialize the key here
         * and register the keyed instance prior to writing */
        // instance_handle = writer.register_instance(instance);

        final long sendPeriodMillis = 1 * 1000;  // 1 second

        for (int samplesWritten = 0;
             !isShutdownRequested() && samplesWritten < getMaxSampleCount();
             samplesWritten++) {
            System.out.println("Writing cft, count " + samplesWritten);

            // Modify the instance to be written here
            if (samplesWritten % 2 == 1) {
                data.name = "ODD";
            } else {
                data.name = "EVEN";
            }

            data.count = samplesWritten;

            // Write data
            writer.write(data, instance_handle);
            try {
                Thread.sleep(sendPeriodMillis);
            } catch (InterruptedException ix) {
                System.err.println("INTERRUPTED");
                break;
            }
        }
    }

    @Override public void close()
    {
        // Delete all entities (DataWriter, Topic, Publisher, DomainParticipant)
        if (participant != null) {
            participant.delete_contained_entities();

            DomainParticipantFactory.get_instance().delete_participant(
                    participant);
        }
    }

    public static void main(String[] args)
    {
        // Create example and run: Uses try-with-resources,
        // publisherApplication.close() automatically called
        try (cftPublisher publisherApplication = new cftPublisher()) {
            publisherApplication.parseArguments(
                    args,
                    ApplicationType.PUBLISHER);
            publisherApplication.addShutdownHook();
            publisherApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
