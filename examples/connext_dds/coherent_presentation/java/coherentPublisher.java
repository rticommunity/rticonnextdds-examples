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
import java.util.Random;

import com.rti.dds.domain.DomainParticipant;
import com.rti.dds.domain.DomainParticipantFactory;
import com.rti.dds.infrastructure.InstanceHandle_t;
import com.rti.dds.infrastructure.StatusKind;
import com.rti.dds.publication.Publisher;
import com.rti.dds.topic.Topic;

// ===========================================================================

public class coherentPublisher extends Application implements AutoCloseable {
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


        /* If you want to change the DataWriter's QoS programmatically
         * rather than using the XML file, you will need to add the
         * following lines to your code and comment out the create_publisher
         * call above.
         */
        // Start changes for coherent_presentation

        /* Get default publisher QoS to customize */
        /*
        PublisherQos publisher_qos = new PublisherQos();
        participant.get_default_publisher_qos(publisher_qos);

        // Topic access scope means that writes from a particular datawriter
        // to multiple instances will be viewed coherently.
        publisher_qos.presentation.access_scope =
            PresentationQosPolicyAccessScopeKind.TOPIC_PRESENTATION_QOS;
        publisher_qos.presentation.coherent_access = true;

        publisher = participant.create_publisher(
            publisher_qos, null,
            StatusKind.STATUS_MASK_NONE);
        */
        // End changes for coherent_presentation

        // Register the datatype to use when creating the Topic
        String typeName = coherentTypeSupport.get_type_name();
        coherentTypeSupport.register_type(participant, typeName);

        /* To customize topic QoS, use
           the configuration file USER_QOS_PROFILES.xml */

        // Create a Topic with a name and a datatype
        Topic topic = Objects.requireNonNull(participant.create_topic(
                "Example coherent",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));

        // This DataWriter writes data on "Example coherent" Topic
        coherentDataWriter writer = (coherentDataWriter) Objects.requireNonNull(
                publisher.create_datawriter(
                        topic,
                        Publisher.DATAWRITER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));


        /* If you want to change the DataWriter's QoS programmatically
         * rather than using the XML file, you will need to add the
         * following lines to your code and comment out the
         * create_datawriter call above.
         */

        // Start changes for coherent_presentation

        // Get default datawriter QoS to customize
        /*
        DataWriterQos datawriter_qos = new DataWriterQos();
        publisher.get_default_datawriter_qos(datawriter_qos);

        datawriter_qos.reliability.kind =
                ReliabilityQosPolicyKind.RELIABLE_RELIABILITY_QOS;
        datawriter_qos.history.depth = 10;

        coherentDataWriter writer = (coherentDataWriter)
            publisher.create_datawriter(
                topic, datawriter_qos,
                null, StatusKind.STATUS_MASK_NONE);
        */
        // End changes for coherent_presentation

        // --- Write --- //

        // Create data sample for writing
        coherent data = new coherent();
        data.id = 0;

        InstanceHandle_t instance_handle = InstanceHandle_t.HANDLE_NIL;
        /*
         * For a data type that has a key, if the same instance is going to
         * be written multiple times, initialize the key here and register
         * the keyed instance prior to writing
         */
        instance_handle = writer.register_instance(data);

        publisher.begin_coherent_changes();
        System.out.println("Begin Coherent Changes");

        final long sendPeriodMillis = 1 * 1000;  // 4 second
        int mod = 0;
        Random rand = new Random();

        for (int samplesWritten = 0;
             !isShutdownRequested() && samplesWritten < getMaxSampleCount();
             samplesWritten++) {
            try {
                Thread.sleep(sendPeriodMillis);
            } catch (InterruptedException ix) {
                System.err.println("INTERRUPTED");
                break;
            }

            mod = samplesWritten % 7;
            if (mod == 6) {
                publisher.begin_coherent_changes();
                System.out.print("Begin Coherent Changes\n");
                continue;
            }

            data.field = (char) ((int) 'a' + mod);
            data.value = rand.nextInt(10);

            System.out.print(
                    "  Updating instance, " + data.field + "->" + data.value
                    + "\n");

            // Write data
            writer.write(data, instance_handle);

            if (mod == 5) {
                publisher.end_coherent_changes();
                System.out.print("End Coherent Changes\n\n");
            }
        }

        writer.unregister_instance(data, instance_handle);
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
        try (coherentPublisher publisherApplication = new coherentPublisher()) {
            publisherApplication.parseArguments(args);
            publisherApplication.addShutdownHook();
            publisherApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
