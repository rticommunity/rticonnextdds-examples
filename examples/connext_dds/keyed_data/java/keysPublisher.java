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

public class keysPublisher extends Application implements AutoCloseable {
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

        // Register type before creating topic
        String typeName = keysTypeSupport.get_type_name();
        keysTypeSupport.register_type(participant, typeName);

        // Create a Topic with a name and a datatype
        Topic topic = Objects.requireNonNull(participant.create_topic(
                "Example keys",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));

        // This DataWriter writes data on "Example keys" Topic
        keysDataWriter writer = (keysDataWriter) Objects.requireNonNull(
                publisher.create_datawriter(
                        topic,
                        Publisher.DATAWRITER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        /* If you want to set the writer_data_lifecycle QoS settings
         * programmatically rather than using the XML, you will need to add
         * the following lines to your code and comment out the
         * create_datawriter call above.
         */
        /*DataWriterQos datawriter_qos = new DataWriterQos();
        publisher.get_default_datawriter_qos(datawriter_qos);

        datawriter_qos.writer_data_lifecycle.autodispose_unregistered_instances
        = false;

        keysDataWriter writer = (keysDataWriter) Objects.requireNonNull(
                publisher.create_datawriter(
                                topic, datawriter_qos,
                                null, StatusKind.STATUS_MASK_NONE));
        */

        // --- Write --- //

        // Create data samples for writing
        keys instance[] = new keys[3];
        instance[0] = new keys();
        instance[1] = new keys();
        instance[2] = new keys();

        InstanceHandle_t handle[] = new InstanceHandle_t[3];
        handle[0] = InstanceHandle_t.HANDLE_NIL;
        handle[1] = InstanceHandle_t.HANDLE_NIL;
        handle[2] = InstanceHandle_t.HANDLE_NIL;

        /* RTI Connext could examine the key fields each time it needs to
         * determine which data-instance is being modified. However, for
         * performance and semantic reasons, it is better for your application
         * to declare all the data-instances it intends to modify prior to
         * actually writing any samples. This is known as registration.
         */

        // In order to register the instances, we must set their associated keys
        // first
        instance[0].code = 0;
        instance[1].code = 1;
        instance[2].code = 2;

        // The keys must have been set before making this call
        System.out.println("Registering instance " + instance[0].code);
        handle[0] = writer.register_instance(instance[0]);

        // Modify the data to be sent here
        instance[0].x = 1000;
        instance[1].x = 2000;
        instance[2].x = 3000;

        // We only will send data over the instances marked as active
        boolean active[] = new boolean[3];
        active[0] = true;
        active[1] = false;
        active[2] = false;

        final long sendPeriodMillis = 1000;  // 1 second

        for (int samplesWritten = 0;
             !isShutdownRequested() && samplesWritten < getMaxSampleCount();
             samplesWritten++) {
            try {
                Thread.sleep(sendPeriodMillis);
            } catch (InterruptedException ix) {
                System.err.println("INTERRUPTED");
                break;
            }

            switch (samplesWritten) {
            case 5: { /* Start sending the second and third instances */
                System.out.println(
                        "----Registering instance" + instance[1].code);
                System.out.println(
                        "----Registering instance" + instance[2].code);
                handle[1] = writer.register_instance(instance[1]);
                handle[2] = writer.register_instance(instance[2]);
                active[1] = true;
                active[2] = true;
            } break;
            case 10: { /* Unregister the second instance */
                System.out.println(
                        "----Unregistering instance" + instance[1].code);
                writer.unregister_instance(instance[1], handle[1]);
                active[1] = false;
            } break;
            case 15: { /* Dispose the third instance */
                System.out.println("----Disposing instance" + instance[2].code);
                writer.dispose(instance[2], handle[2]);
                active[2] = false;
            } break;
            }

            // Modify the data to be sent here
            instance[0].y = samplesWritten;
            instance[1].y = samplesWritten;
            instance[2].y = samplesWritten;

            for (int i = 0; i < 3; ++i) {
                if (active[i]) {
                    System.out.println(
                            "Writing instance " + instance[i].code + ", x: "
                            + instance[i].x + ", y: " + instance[i].y);
                    writer.write(instance[i], handle[i]);
                }
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
        try (keysPublisher publisherApplication = new keysPublisher()) {
            publisherApplication.parseArguments(args);
            publisherApplication.addShutdownHook();
            publisherApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
