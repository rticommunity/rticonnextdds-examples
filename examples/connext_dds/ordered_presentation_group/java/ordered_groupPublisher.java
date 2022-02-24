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

public class ordered_groupPublisher
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
        String typeName = ordered_groupTypeSupport.get_type_name();
        ordered_groupTypeSupport.register_type(participant, typeName);

        // Start changes for Ordered Presentation Group example

        // TOPICS
        Topic topic1 = Objects.requireNonNull(participant.create_topic(
                "Topic1",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null /* listener */,
                StatusKind.STATUS_MASK_NONE));

        Topic topic2 = Objects.requireNonNull(participant.create_topic(
                "Topic2",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null /* listener */,
                StatusKind.STATUS_MASK_NONE));

        Topic topic3 = Objects.requireNonNull(participant.create_topic(
                "Topic3",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null /* listener */,
                StatusKind.STATUS_MASK_NONE));

        // DATAWRITERS
        ordered_groupDataWriter writer1 =
                (ordered_groupDataWriter) Objects.requireNonNull(
                        publisher.create_datawriter(
                                topic1,
                                Publisher.DATAWRITER_QOS_DEFAULT,
                                null /* listener */,
                                StatusKind.STATUS_MASK_NONE));

        ordered_groupDataWriter writer2 =
                (ordered_groupDataWriter) Objects.requireNonNull(
                        publisher.create_datawriter(
                                topic2,
                                Publisher.DATAWRITER_QOS_DEFAULT,
                                null /* listener */,
                                StatusKind.STATUS_MASK_NONE));

        ordered_groupDataWriter writer3 =
                (ordered_groupDataWriter) Objects.requireNonNull(
                        publisher.create_datawriter(
                                topic3,
                                Publisher.DATAWRITER_QOS_DEFAULT,
                                null /* listener */,
                                StatusKind.STATUS_MASK_NONE));


        // Instances

        // Create data sample for writing
        ordered_group instance1 = new ordered_group();
        ordered_group instance2 = new ordered_group();
        ordered_group instance3 = new ordered_group();

        // End changes for Ordered Presentation Example

        InstanceHandle_t instance_handle = InstanceHandle_t.HANDLE_NIL;

        final long sendPeriodMillis = 1000;  // 1 seconds

        for (int samplesWritten = 0;
             !isShutdownRequested() && samplesWritten < getMaxSampleCount();
             samplesWritten++) {
            System.out.println(
                    "Writing ordered_group, count " + samplesWritten);

            // Start changes for Ordered Presentation Example
            // Modify the instance to be written here
            // Instance 1
            instance1.message = "First sample, Topic 1 by DataWriter number 1";
            writer1.write(instance1, instance_handle);

            instance1.message = "Second sample, Topic 1 by DataWriter number 1";
            writer1.write(instance1, instance_handle);

            // Instance 2
            instance2.message = "First sample, Topic 2 by DataWriter number 2";
            writer2.write(instance2, instance_handle);

            instance2.message = "Second sample, Topic 2 by DataWriter number 2";
            writer2.write(instance2, instance_handle);

            // Instance 3
            instance3.message = "First sample, Topic 3 by DataWriter number 3";
            writer3.write(instance3, instance_handle);

            instance3.message = "Second sample, Topic 3 by DataWriter number 3";
            writer3.write(instance3, instance_handle);

            try {
                Thread.sleep(sendPeriodMillis);
            } catch (InterruptedException ix) {
                System.err.println("INTERRUPTED");
                break;
            }

            // End changes for Ordered Presentation Example
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
        try (ordered_groupPublisher publisherApplication =
                     new ordered_groupPublisher()) {
            publisherApplication.parseArguments(args);
            publisherApplication.addShutdownHook();
            publisherApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
