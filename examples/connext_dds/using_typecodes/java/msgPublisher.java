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
import com.rti.dds.domain.DomainParticipantQos;
import com.rti.dds.domain.DomainParticipantFactory;
import com.rti.dds.infrastructure.InstanceHandle_t;
import com.rti.dds.infrastructure.StatusKind;
import com.rti.dds.publication.Publisher;
import com.rti.dds.topic.Topic;

public class msgPublisher extends Application implements AutoCloseable {
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

        /*
         * If you want to change the type_code_max_serialized_length
         * programmatically (e.g., to 3070) rather than using the XML file,
         * you will need to add the following lines to your code and comment
         * out the create_participant call above.
         */

        /*
        DomainParticipantQos participant_qos = new DomainParticipantQos();
        DomainParticipantFactory.TheParticipantFactory
                .get_default_participant_qos(participant_qos);

        participant_qos.resource_limits.type_code_max_serialized_length = 3070;

        participant = Objects.requireNonNull(
                DomainParticipantFactory.get_instance().create_participant(
                        getDomainId(),
                        participant_qos,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));
        */

        // A Publisher allows an application to create one or more DataWriters
        Publisher publisher =
                Objects.requireNonNull(participant.create_publisher(
                        DomainParticipant.PUBLISHER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        // Register type before creating topic
        String typeName = msgTypeSupport.get_type_name();
        msgTypeSupport.register_type(participant, typeName);

        // Create a Topic with a name and a datatype
        Topic topic = Objects.requireNonNull(participant.create_topic(
                "Example msg",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));

        // This DataWriter writes data on "Example msg" Topic
        msgDataWriter writer = (msgDataWriter) Objects.requireNonNull(
                publisher.create_datawriter(
                        topic,
                        Publisher.DATAWRITER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        // Create data sample for writing
        msg data = new msg();

        InstanceHandle_t instance_handle = InstanceHandle_t.HANDLE_NIL;
        /*
         * For a data type that has a key, if the same instance is going to
         * be written multiple times, initialize the key here and register
         * the keyed instance prior to writing
         */
        // instance_handle = writer.register_instance(data);

        final long sendPeriodMillis = 4 * 1000;  // 4 seconds

        for (int samplesWritten = 0;
             !isShutdownRequested() && samplesWritten < getMaxSampleCount();
             samplesWritten++) {
            System.out.println("Writing msg, count " + samplesWritten);

            /* Modify the instance to be written here */
            data.count = samplesWritten;

            /* Write data */
            writer.write(data, instance_handle);
            try {
                Thread.sleep(sendPeriodMillis);
            } catch (InterruptedException ix) {
                System.err.println("INTERRUPTED");
                break;
            }
        }

        // writer.unregister_instance(data, instance_handle);
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
        try (msgPublisher publisherApplication = new msgPublisher()) {
            publisherApplication.parseArguments(args);
            publisherApplication.addShutdownHook();
            publisherApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
