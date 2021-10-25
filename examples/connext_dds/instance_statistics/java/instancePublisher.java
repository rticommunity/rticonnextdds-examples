/*
 * (c) Copyright, Real-Time Innovations, 2021.  All rights reserved.
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the software solely for use with RTI Connext DDS. Licensee may
 * redistribute copies of the software provided that all such copies are subject
 * to this license. The software is provided "as is", with no warranty of any
 * type, including any warranty for fitness for any purpose. RTI is under no
 * obligation to maintain or support the software. RTI shall not be liable for
 * any incidental or consequential damages arising out of the use or inability
 * to use the software.
 */

import java.util.Objects;

import com.rti.dds.domain.DomainParticipant;
import com.rti.dds.domain.DomainParticipantFactory;
import com.rti.dds.infrastructure.InstanceHandle_t;
import com.rti.dds.infrastructure.StatusKind;
import com.rti.dds.publication.*;
import com.rti.dds.topic.Topic;

/** 
* Simple example showing all Connext code in one place for readability.
*/
public class instancePublisher extends Application implements AutoCloseable {

    // Usually one per application
    private DomainParticipant participant = null;

    private void runApplication() {
        // Start communicating in a domain
        participant = Objects.requireNonNull(
            DomainParticipantFactory.get_instance().create_participant(
                getDomainId(),
                DomainParticipantFactory.PARTICIPANT_QOS_DEFAULT,
                null, // listener
                StatusKind.STATUS_MASK_NONE));

        // A Publisher allows an application to create one or more DataWriters
        Publisher publisher = Objects.requireNonNull(
            participant.create_publisher(
                DomainParticipant.PUBLISHER_QOS_DEFAULT,
                null, // listener
                StatusKind.STATUS_MASK_NONE));

        // Register the datatype to use when creating the Topic
        String typeName = instanceTypeSupport.get_type_name();
        instanceTypeSupport.register_type(participant, typeName);

        // Create a Topic with a name and a datatype
        Topic topic = Objects.requireNonNull(
            participant.create_topic(
                "Example instance",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null, // listener
                StatusKind.STATUS_MASK_NONE));

        // This DataWriter writes data on "Example instance" Topic
        instanceDataWriter writer = (instanceDataWriter) Objects.requireNonNull(
            publisher.create_datawriter(
                topic,
                Publisher.DATAWRITER_QOS_DEFAULT,
                null, // listener
                StatusKind.STATUS_MASK_NONE));

        // Create data sample for writing
        instance data = new instance();
        InstanceHandle_t handle;
        DataWriterCacheStatus status = new DataWriterCacheStatus();
        for (int samplesWritten = 0; !isShutdownRequested()
        && samplesWritten < getMaxSampleCount(); samplesWritten++) {

            // Modify the data to be written here
            data.x = (short) samplesWritten;

            System.out.println("Writing instance, count " + samplesWritten);
            handle = writer.register_instance(data);
            writer.write(data, handle);
            try {
                final long sendPeriodMillis = 1000; // 1 second
                Thread.sleep(sendPeriodMillis);
            } catch (InterruptedException ix) {
                System.err.println("INTERRUPTED");
                break;
            }
            if (samplesWritten % 2 == 0) {
                /* Unregister the instance */
                writer.unregister_instance(data, handle);
            } else if (samplesWritten % 3 == 0) {
                /* Dispose the instance */
                writer.dispose(data, handle);
            }
            writer.get_datawriter_cache_status(status);
            System.out.println("Instance statistics:\n"
                    + "\t alive_instance_count "
                    + status.alive_instance_count + "\n"
                    + "\t unregistered_instance_count "
                    + status.unregistered_instance_count + "\n"
                    + "\t disposed_instance_count "
                    + status.disposed_instance_count);

        }
    }

    @Override
    public void close() {
        // Delete all entities (DataWriter, Topic, Publisher, DomainParticipant)
        if (participant != null) {
            participant.delete_contained_entities();

            DomainParticipantFactory.get_instance()
            .delete_participant(participant);
        }
    }

    public static void main(String[] args) {
        // Create example and run: Uses try-with-resources,
        // publisherApplication.close() automatically called
        try (instancePublisher publisherApplication = new instancePublisher()) {
            publisherApplication.parseArguments(args);
            publisherApplication.addShutdownHook();
            publisherApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at application exit.
        DomainParticipantFactory.finalize_instance();
    }
}