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


public class batch_dataPublisher extends Application implements AutoCloseable {
    // Usually one per application
    private DomainParticipant participant = null;

    private void runApplication()
    {
        String profile_name = null;
        String library_name = "batching_Library";

        if (isTurboModeActivated()) {
            profile_name = "turbo_mode_profile";
            System.out.println("Turbo Mode enable");
        } else {
            profile_name = "batch_profile";
            System.out.println("Manual batching enable");
        }

        // To customize participant QoS, use the configuration file
        // USER_QOS_PROFILES.xml
        participant = Objects.requireNonNull(
                DomainParticipantFactory.TheParticipantFactory
                        .create_participant_with_profile(
                                getDomainId(),
                                library_name,
                                profile_name,
                                null /* listener */,
                                StatusKind.STATUS_MASK_NONE));

        // A Publisher allows an application to create one or more DataWriters
        Publisher publisher = Objects.requireNonNull(
                participant.create_publisher_with_profile(
                        library_name,
                        profile_name,
                        null /* listener */,
                        StatusKind.STATUS_MASK_NONE));

        // Register the datatype to use when creating the Topic
        String typeName = batch_dataTypeSupport.get_type_name();
        batch_dataTypeSupport.register_type(participant, typeName);

        // Create a Topic with a name and a datatype
        Topic topic = Objects.requireNonNull(participant.create_topic(
                "Example batch_data",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));

        // This DataWriter writes data on "Example batch_data" Topic
        batch_dataDataWriter writer =
                (batch_dataDataWriter) Objects.requireNonNull(
                        publisher.create_datawriter_with_profile(
                                topic,
                                library_name,
                                profile_name,
                                null,  // listener
                                StatusKind.STATUS_MASK_NONE));

        // Create data sample for writing
        batch_data data = new batch_data();

        InstanceHandle_t instance_handle = InstanceHandle_t.HANDLE_NIL;
        /* For a data type that has a key, if the same instance is going to
         * be written multiple times, initialize the key here
         *and register the keyed instance prior to writing
         */
        // instance_handle = writer.register_instance(data);

        long sendPeriodMillis = 1 * 1000;  // 1 second
        if (isTurboModeActivated()) {
            sendPeriodMillis = 0;
        }

        for (int samplesWritten = 0;
             !isShutdownRequested() && samplesWritten < getMaxSampleCount();
             samplesWritten++) {
            System.out.println("Writing batch_data, count " + samplesWritten);

            // Modify the instance to be written here
            data.x = samplesWritten;

            // Write data
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
        try (batch_dataPublisher publisherApplication =
                     new batch_dataPublisher()) {
            publisherApplication.parseArguments(args);
            publisherApplication.addShutdownHook();
            publisherApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
