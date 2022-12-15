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


public class orderedPublisher extends Application implements AutoCloseable {
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
        Publisher publisher = Objects.requireNonNull(
                participant.create_publisher_with_profile(
                        "ordered_Library",
                        "ordered_Profile_subscriber_instance",
                        null,
                        StatusKind.STATUS_MASK_NONE));


        /*
         * If you want to change the Publisher's QoS programmatically rather
         * than using the XML file, you will need to add the following lines
         * to your code and comment out the create_publisher call above.
         *
         * In this case, we set the presentation publish mode ordered in the
         * topic.
         */
        // Get default publisher QoS to customize
        /*
        PublisherQos publisher_qos = new PublisherQos();
        participant.get_default_publisher_qos(publisher_qos);

        publisher_qos.presentation.access_scope =
                PresentationQosPolicyAccessScopeKind.TOPIC_PRESENTATION_QOS;
        publisher_qos.presentation.ordered_access = true;

        publisher = Objects.requireNonNull(
                participant.create_publisher(
                    publisher_qos,
                    null,
                    StatusKind.STATUS_MASK_NONE));
        if (publisher == null) {
            System.err.println("create_publisher error\n");
            return;
        }
        */
        // End changes for ordered presentation

        // Register type before creating topic
        String typeName = orderedTypeSupport.get_type_name();
        orderedTypeSupport.register_type(participant, typeName);

        // Create a Topic with a name and a datatype
        Topic topic = Objects.requireNonNull(participant.create_topic(
                "Example ordered",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));

        // This DataWriter writes data on "Example ordered" Topic
        orderedDataWriter writer = (orderedDataWriter) Objects.requireNonNull(
                publisher.create_datawriter(
                        topic,
                        Publisher.DATAWRITER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        // Start changes for ordered presentation
        ordered instance0 = new ordered();
        ordered instance1 = new ordered();
        InstanceHandle_t handle0 = InstanceHandle_t.HANDLE_NIL;
        InstanceHandle_t handle1 = InstanceHandle_t.HANDLE_NIL;

        /*
         * For a data type that has a key, if the same instance is going to
         * be written multiple times, initialize the key here and register
         * the keyed instance prior to writing
         */
        instance0.id = 0;
        instance1.id = 1;

        handle0 = writer.register_instance(instance0);
        handle1 = writer.register_instance(instance1);

        final long sendPeriodMillis = 1000;  // 1 second

        for (int samplesWritten = 0;
             !isShutdownRequested() && samplesWritten < getMaxSampleCount();
             samplesWritten++) {
            instance0.value = samplesWritten;
            instance1.value = samplesWritten;

            System.out.println("writing instance0, value-> " + instance0.value);
            writer.write(instance0, handle0);

            System.out.println("writing instance1, value-> " + instance1.value);
            writer.write(instance1, handle1);

            try {
                Thread.sleep(sendPeriodMillis);
            } catch (InterruptedException ix) {
                System.err.println("INTERRUPTED");
                break;
            }
        }

        writer.unregister_instance(instance0, handle0);
        writer.unregister_instance(instance1, handle1);
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
        try (orderedPublisher publisherApplication = new orderedPublisher()) {
            publisherApplication.parseArguments(args);
            publisherApplication.addShutdownHook();
            publisherApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
