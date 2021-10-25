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

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Arrays;

import com.rti.dds.domain.*;
import com.rti.dds.infrastructure.*;
import com.rti.dds.publication.*;
import com.rti.dds.topic.*;
import com.rti.ndds.config.*;

// ===========================================================================

public class NetworkCapturePublisher {
    // -----------------------------------------------------------------------
    // Public Methods
    // -----------------------------------------------------------------------

    public static void main(String[] args) {
        // Enable network capture.
        //
        // This must be called before:
        //   - Any other network capture function is called.
        //   - Creating the participants for which we want to capture traffic.
        if (!com.rti.ndds.utility.NetworkCapture.enable()) {
            System.err.println("Error enabling network capture");
        }

        int domainId = 0;
        if (args.length >= 1) {
            domainId = Integer.valueOf(args[0]).intValue();
        }

        int sampleCount = 0; // 0 means infinite loop
        if (args.length >= 2) {
            sampleCount = Integer.valueOf(args[1]).intValue();
        }

        publisherMain(domainId, sampleCount);

        // Disable network capture.
        //
        // This must be:
        //   - The last network capture function that is called.
        //
        if (!com.rti.ndds.utility.NetworkCapture.disable()) {
            System.err.println("Error disabling network capture");
        }
    }

    // -----------------------------------------------------------------------
    // Private Methods
    // -----------------------------------------------------------------------

    // --- Constructors: -----------------------------------------------------
    private NetworkCapturePublisher() {
        super();
    }

    // -----------------------------------------------------------------------
    private static void publisherMain(int domainId, int sampleCount) {

        DomainParticipant participant = null;
        Publisher publisher = null;
        Topic topic = null;
        NetworkCaptureDataWriter writer = null;

        try {
            // Start capturing traffic for all participants.
            //
            // All participants: those already created and those yet to be
            // created.
            // Default parameters: all transports and some other sane defaults.
            //
            // A capture file will be created for each participant. The capture
            // file will start with the prefix "publisher" and continue with a
            // suffix dependent on the participant's GUID.
            if (!com.rti.ndds.utility.NetworkCapture.start("publisher")) {
                System.err.println("Error starting network capture");
            }

            participant = DomainParticipantFactory.TheParticipantFactory.
                    create_participant(
                            domainId,
                            DomainParticipantFactory.PARTICIPANT_QOS_DEFAULT,
                            null, /* listener */
                            StatusKind.STATUS_MASK_NONE);

            publisher = participant.create_publisher(
                    DomainParticipant.PUBLISHER_QOS_DEFAULT,
                    null, /* listener */
                    StatusKind.STATUS_MASK_NONE);

            String typeName = NetworkCaptureTypeSupport.get_type_name();
            NetworkCaptureTypeSupport.register_type(participant, typeName);

            topic = participant.create_topic(
                    "Network capture shared memory example",
                    typeName,
                    DomainParticipant.TOPIC_QOS_DEFAULT,
                    null, /* listener */
                    StatusKind.STATUS_MASK_NONE);

            writer = (NetworkCaptureDataWriter) publisher.create_datawriter(
                        topic,
                        Publisher.DATAWRITER_QOS_DEFAULT,
                        null, /* listener */
                        StatusKind.STATUS_MASK_NONE);

            NetworkCapture instance = new NetworkCapture();
            InstanceHandle_t instance_handle = InstanceHandle_t.HANDLE_NIL;

            for (int count = 0;
                    (sampleCount == 0) || (count < sampleCount);
                    ++count) {
                System.out.println("Writing NetworkCapture, count " + count);
                instance.msg = "Hello World! (" + count + ")";

               // Here we are going to pause capturing for some samples.
               // The resulting pcap file will not contain them.
                if (count == 4
                        && !com.rti.ndds.utility.NetworkCapture.pause()) {
                    System.err.println("Error pausing network capture");
                } else if (count == 6
                        && !com.rti.ndds.utility.NetworkCapture.resume()) {
                    System.err.println("Error resuming network capture");
                }

                writer.write(instance, InstanceHandle_t.HANDLE_NIL);
                try {
                    Thread.sleep(1000); // 1 second
                } catch (InterruptedException ix) {
                    System.err.println("INTERRUPTED");
                    break;
                }
            }
        } finally {

            // Before deleting the participants that are capturing, we must stop
            // network capture for them.
            if (!com.rti.ndds.utility.NetworkCapture.stop()) {
                System.err.println("Error stopping network capture");
            }
            if(participant != null) {
                participant.delete_contained_entities();

                DomainParticipantFactory.TheParticipantFactory.
                    delete_participant(participant);
            }
            DomainParticipantFactory.finalize_instance();
        }
    }
}
