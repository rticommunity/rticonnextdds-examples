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

public class deadline_contentfilterPublisher
        extends Application implements AutoCloseable {
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
        String typeName = deadline_contentfilterTypeSupport.get_type_name();
        deadline_contentfilterTypeSupport.register_type(participant, typeName);

        // Create a Topic with a name and a datatype
        Topic topic = Objects.requireNonNull(participant.create_topic(
                "Example deadline_contentfilter",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));

        // --- Create writer --- //

        /* To customize data writer QoS, use
           the configuration file USER_QOS_PROFILES.xml */

        // This DataWriter writes data on "Example deadline_contentfilter" Topic
        deadline_contentfilterDataWriter writer =
                (deadline_contentfilterDataWriter) Objects.requireNonNull(
                        publisher.create_datawriter(
                                topic,
                                Publisher.DATAWRITER_QOS_DEFAULT,
                                null,  // listener
                                StatusKind.STATUS_MASK_NONE));

        /* If you want to change the DataWriter's QoS programmatically rather
         * than using the XML file, you will need to add the following lines to
         * your code and comment out the create_datawriter call above.
         *
         * In this case, we set the deadline period to 1.5 seconds to trigger
         * a deadline if the DataWriter does not update often enough.
         */
        /*
        DataWriterQos datawriter_qos = new DataWriterQos();
        publisher.get_default_datawriter_qos(datawriter_qos);

        // Set deadline QoS to 1.5sec
        datawriter_qos.deadline.period.sec = 1;
        datawriter_qos.deadline.period.nanosec = 500000000;

        writer = (deadlineDataWriter) Objects.requireNonNull(
            publisher.create_datawriter(
                topic,
                datawriter_qos,
                null,
                StatusKind.OFFERED_DEADLINE_MISSED_STATUS));
        */

        // --- Write --- //

        // Create two instances.
        deadline_contentfilter instance0 = new deadline_contentfilter();
        deadline_contentfilter instance1 = new deadline_contentfilter();

        InstanceHandle_t handle0 = InstanceHandle_t.HANDLE_NIL;
        InstanceHandle_t handle1 = InstanceHandle_t.HANDLE_NIL;

        instance0.code = 0;
        instance1.code = 1;

        /* For data type that has key, if the same instance is going to be
            written multiple times, initialize the key here
            and register the keyed instance prior to writing */
        handle0 = writer.register_instance(instance0);
        handle1 = writer.register_instance(instance1);

        final long sendPeriodMillis = 1000;

        for (int samplesWritten = 0;
             !isShutdownRequested() && samplesWritten < getMaxSampleCount();
             samplesWritten++) {
            try {
                Thread.sleep(sendPeriodMillis);
            } catch (InterruptedException ix) {
                System.err.println("INTERRUPTED");
                break;
            }

            instance0.x++;
            instance0.y++;
            instance1.x++;
            instance1.y++;

            System.out.print(
                    "Writing instance0, x = " + instance0.x
                    + ", y = " + instance0.y + "\n");
            writer.write(instance0, handle0);

            if (samplesWritten < 8) {
                System.out.print(
                        "Writing instance1, x = " + instance1.x
                        + ", y = " + instance1.y + "\n");
                writer.write(instance1, handle1);
            } else if (samplesWritten == 8) {
                System.out.print("Stopping writes to instance1\n");
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
        try (deadline_contentfilterPublisher publisherApplication =
                     new deadline_contentfilterPublisher()) {
            publisherApplication.parseArguments(args);
            publisherApplication.addShutdownHook();
            publisherApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
