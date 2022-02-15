/*******************************************************************************
 (c) 2021 Copyright, Real-Time Innovations, Inc.  All rights reserved.
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


public class NetworkCapturePublisher
        extends Application implements AutoCloseable {
    // Usually one per application
    private DomainParticipant participant = null;

    private void runApplication()
    {
        if (!com.rti.ndds.utility.NetworkCapture.enable()) {
            System.err.println("Error enabling network capture");
        }

        if (!com.rti.ndds.utility.NetworkCapture.start("publisher")) {
            System.err.println("Error starting network capture");
        }

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
        String typeName = NetworkCaptureTypeSupport.get_type_name();
        NetworkCaptureTypeSupport.register_type(participant, typeName);

        // Create a Topic with a name and a datatype
        Topic topic = Objects.requireNonNull(participant.create_topic(
                "Network capture shared memory example",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null, /* listener */
                StatusKind.STATUS_MASK_NONE));

        // This DataWriter writes data on "Example NetworkCapture" Topic
        NetworkCaptureDataWriter writer =
                (NetworkCaptureDataWriter) Objects.requireNonNull(
                        publisher.create_datawriter(
                                topic,
                                Publisher.DATAWRITER_QOS_DEFAULT,
                                null,  // listener
                                StatusKind.STATUS_MASK_NONE));

        NetworkCapture data = new NetworkCapture();
        InstanceHandle_t instance_handle = InstanceHandle_t.HANDLE_NIL;

        for (int samplesWritten = 0;
             !isShutdownRequested() && samplesWritten < getMaxSampleCount();
             samplesWritten++) {
            System.out.println(
                    "Writing NetworkCapture, count " + samplesWritten);
            data.msg = "Hello World! (" + samplesWritten + ")";

            // Here we are going to pause capturing for some samples.
            // The resulting pcap file will not contain them.
            if (samplesWritten == 4
                && !com.rti.ndds.utility.NetworkCapture.pause()) {
                System.err.println("Error pausing network capture");
            } else if (
                    samplesWritten == 6
                    && !com.rti.ndds.utility.NetworkCapture.resume()) {
                System.err.println("Error resuming network capture");
            }

            writer.write(data, instance_handle);
            try {
                Thread.sleep(1000);  // 1 second
            } catch (InterruptedException ix) {
                System.err.println("INTERRUPTED");
                break;
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
        try (NetworkCapturePublisher publisherApplication =
                     new NetworkCapturePublisher()) {
            publisherApplication.parseArguments(args);
            publisherApplication.addShutdownHook();
            publisherApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
