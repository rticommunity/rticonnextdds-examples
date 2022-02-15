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

public class asyncPublisher extends Application implements AutoCloseable {
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
        String typeName = asyncTypeSupport.get_type_name();
        asyncTypeSupport.register_type(participant, typeName);

        // Create a Topic with a name and a datatype
        Topic topic = Objects.requireNonNull(participant.create_topic(
                "Example async",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));

        // This DataWriter writes data on "Example async" Topic
        asyncDataWriter writer = (asyncDataWriter) Objects.requireNonNull(
                publisher.create_datawriter(
                        topic,
                        Publisher.DATAWRITER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        /*
         * If you want to change the DataWriter's QoS programmatically
         * rather than using the XML file, you will need to add the
         * following lines to your code and comment out the
         * create_datawriter call above.
         *
         * In this case, we set the publish mode qos to asynchronous publish
         * mode, which enables asynchronous publishing. We also set the flow
         * controller to be the fixed rate flow controller, and we increase
         * the history depth.
         */

        // // Start changes for Asynchronous_Publication

        // --- Create writer --- //

        /* Get default datawriter QoS to customize */
        // DataWriterQos datawriter_qos = new DataWriterQos();
        // publisher.get_default_datawriter_qos(datawriter_qos);
        //
        // // Since samples are only being sent once per second, datawriter
        // // will need to keep them on queue. History defaults to only
        // // keeping the last sample enqueued, so we increase that here.
        // datawriter_qos.history.depth = 12;
        //
        // // Set flowcontroller for datawriter
        // datawriter_qos.publish_mode.kind =
        // PublishModeQosPolicyKind.ASYNCHRONOUS_PUBLISH_MODE_QOS;
        // datawriter_qos.publish_mode.flow_controller_name =
        // FlowController.FIXED_RATE_FLOW_CONTROLLER_NAME;
        //
        // /* To create datawriter with default QoS, use
        // Publisher.DATAWRITER_QOS_DEFAULT instead of datawriter_qos */
        // writer = (asyncDataWriter)
        // publisher.create_datawriter(
        // topic, datawriter_qos,
        // null /* listener */, StatusKind.STATUS_MASK_NONE);
        // // End changes for Asynchronous_Publication
        // --- Write --- //

        // Create data sample for writing
        async data = new async();

        InstanceHandle_t instance_handle = InstanceHandle_t.HANDLE_NIL;
        /*
         * For a data type that has a key, if the same instance is going to
         * be written multiple times, initialize the key here and register
         * the keyed instance prior to writing
         */
        // instance_handle = writer.register_instance(instance);

        final long sendPeriodMillis = 500;  // 1 sample per 0.5 second

        for (int samplesWritten = 0;
             !isShutdownRequested() && samplesWritten < getMaxSampleCount();
             samplesWritten++) {
            System.out.println("Writing async, count " + samplesWritten);

            // Modify the instance to be written here
            data.x = samplesWritten;

            /* Write data */
            writer.write(data, instance_handle);
            try {
                Thread.sleep(sendPeriodMillis);
            } catch (InterruptedException ix) {
                System.err.println("INTERRUPTED");
                break;
            }
        }
        // writer.unregister_instance(instance, instance_handle);
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
        try (asyncPublisher publisherApplication = new asyncPublisher()) {
            publisherApplication.parseArguments(args);
            publisherApplication.addShutdownHook();
            publisherApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
