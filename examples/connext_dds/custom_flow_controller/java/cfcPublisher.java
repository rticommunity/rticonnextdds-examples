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

public class cfcPublisher extends Application implements AutoCloseable {
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


        /* Start changes for custom_flowcontroller */

        /* If you want to change the Participant's QoS programmatically
         * rather than using the XML file, you will need to add the
         * following lines to your code and comment out the
         * create_participant call above.
         */
        /*
            DomainParticipantQos participant_qos = new DomainParticipantQos();
            DomainParticipantFactory.TheParticipantFactory.
                    get_default_participant_qos(participant_qos);

            // By default, data will be sent via shared memory _and_ UDPv4.
            // Because the flowcontroller limits writes across all interfaces,
            // this halves the effective send rate.  To avoid this, we enable
            // only the UDPv4 transport
            participant_qos.transport_builtin.mask = TransportBuiltinKind.UDPv4;

            // To create participant with default QoS, use
            // DomainParticipantFactory.PARTICIPANT_QOS_DEFAULT instead of
            // participant_qos
            participant = Objects.requireNonNull(
                DomainParticipantFactory.get_instance().create_participant(
                    getDomainId(),
                    participant_qos,
                    null,
                    StatusKind.STATUS_MASK_NONE));
        */
        /* End changes for Custom_Flowcontroller */

        // A Publisher allows an application to create one or more DataWriters
        Publisher publisher =
                Objects.requireNonNull(participant.create_publisher(
                        DomainParticipant.PUBLISHER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        // Register type before creating topic
        String typeName = cfcTypeSupport.get_type_name();
        cfcTypeSupport.register_type(participant, typeName);

        // Create a Topic with a name and a datatype
        Topic topic = Objects.requireNonNull(participant.create_topic(
                "Example cfc",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));

        // This DataWriter writes data on "Example cfc" Topic
        cfcDataWriter writer = (cfcDataWriter) Objects.requireNonNull(
                publisher.create_datawriter(
                        topic,
                        Publisher.DATAWRITER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));


        /* Start changes for custom_flowcontroller */

        /* If you want to change the Datawriter's QoS programmatically
         * rather than using the XML file, you will need to add the
         * following lines to your code and comment out the
         * create_datawriter call above.
         *
         * In this case we create the flowcontroller and the neccesary QoS
         * for the datawriter.
         */
        /*
        String cfc_name = "custom_flowcontroller";
        // Create and configure flowcontroller properties
        FlowControllerProperty_t custom_fcp =
                new FlowControllerProperty_t();
        participant.get_default_flowcontroller_property(custom_fcp);

        // Don't allow too many tokens to accumulate
        custom_fcp.token_bucket.max_tokens =
            custom_fcp.token_bucket.tokens_added_per_period = 2;
        custom_fcp.token_bucket.tokens_leaked_per_period =
                ResourceLimitsQosPolicy.LENGTH_UNLIMITED;

        // 100ms
        custom_fcp.token_bucket.period.sec = 0;
        custom_fcp.token_bucket.period.nanosec = 100000000;

        // The sample size is 1000, but the minimum bytes_per_token is 1024.
        // Furthermore, we want to allow some overhead.
        custom_fcp.token_bucket.bytes_per_token = 1024;

        // So, in summary, each token can be used to send about one message,
        // and we get 2 tokens every 100ms, so this limits transmissions to
        // about 20 messages per second.

        // Create flowcontroller and set properties
        FlowController cfc = participant.create_flowcontroller(
                cfc_name, custom_fcp);
        cfc.set_property(custom_fcp);

        // --- Create writer --- //

        // Get default datawriter QoS to customize
        DataWriterQos datawriter_qos = new DataWriterQos();
        publisher.get_default_datawriter_qos(datawriter_qos);

        // As an alternative to increasing history depth, we can just
        // set the qos to keep all samples
        datawriter_qos.history.kind =
                HistoryQosPolicyKind.KEEP_ALL_HISTORY_QOS;

        // Set flowcontroller for datawriter
        datawriter_qos.publish_mode.kind =
                PublishModeQosPolicyKind.cfcHRONOUS_PUBLISH_MODE_QOS;
        datawriter_qos.publish_mode.flow_controller_name = cfc_name;

        writer = (cfcDataWriter)
                publisher.create_datawriter(
                    topic, datawriter_qos,
                    null, StatusKind.STATUS_MASK_NONE);
        */
        // End changes to custom_flowcontroller

        // Create data sample for writing
        cfc instance = new cfc();

        InstanceHandle_t instance_handle = InstanceHandle_t.HANDLE_NIL;
        /* For a data type that has a key, if the same instance is
         * going to be written multiple times, initialize the key here
         * and register the keyed instance prior to writing */
        // instance_handle = writer.register_instance(instance);

        long sendPeriodMillis = 1 * 1000;  // 1 second

        char str[] = new char[1000];
        for (int i = 0; i < 1000; ++i)
            str[i] = 'a';
        String data = new String(str);

        for (int samplesWritten = 0;
             !isShutdownRequested() && samplesWritten < getMaxSampleCount();
             samplesWritten++) {
            /* Changes for Custom_Flowcontroller */
            /* Simulate bursty writer */

            try {
                Thread.sleep(sendPeriodMillis);
            } catch (InterruptedException ix) {
                System.err.println("INTERRUPTED");
                break;
            }

            for (int i = 0; i < 10; ++i) {
                int sample = samplesWritten * 10 + i;
                System.out.println("Writing cfc, sample " + sample);
                instance.x = sample;
                instance.str = data;
                try {
                    writer.write(instance, instance_handle);
                } catch (Exception e) {
                    System.out.println("writer error: " + e);
                }
            }
        }

        sendPeriodMillis = 4 * 1000;

        try {
            Thread.sleep(sendPeriodMillis);
        } catch (InterruptedException ix) {
            System.err.println("INTERRUPTED");
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
        try (cfcPublisher publisherApplication = new cfcPublisher()) {
            publisherApplication.parseArguments(args);
            publisherApplication.addShutdownHook();
            publisherApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
