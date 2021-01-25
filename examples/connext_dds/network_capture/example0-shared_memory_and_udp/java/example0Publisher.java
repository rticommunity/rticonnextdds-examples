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
import com.rti.ndds.utility.*;

// ===========================================================================

public class example0Publisher {
    // -----------------------------------------------------------------------
    // Public Methods
    // -----------------------------------------------------------------------

    public static void main(String[] args) {
        // Enable network capture.
        //
        // This must be called before:
        //   - Any other network capture function is called.
        //   - Creating the participants for which we want to capture traffic.
        if (!NetworkCapture.enable()) {
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
        if (!NetworkCapture.disable()) {
            System.err.println("Error disabling network capture");
        }
    }

    // -----------------------------------------------------------------------
    // Private Methods
    // -----------------------------------------------------------------------

    // --- Constructors: -----------------------------------------------------
    private example0Publisher() {
        super();
    }

    // -----------------------------------------------------------------------
    private static void publisherMain(int domainId, int sampleCount) {

        DomainParticipant participant = null;
        Publisher publisher = null;
        Topic topic = null;
        example0DataWriter writer = null;

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
            if (!NetworkCapture.start("publisher")) {
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

            String typeName = example0TypeSupport.get_type_name();
            example0TypeSupport.register_type(participant, typeName);

            topic = participant.create_topic(
                    "example0 using network capture Java API",
                    typeName,
                    DomainParticipant.TOPIC_QOS_DEFAULT,
                    null, /* listener */
                    StatusKind.STATUS_MASK_NONE);

            writer = (example0DataWriter) publisher.create_datawriter(
                        topic,
                        Publisher.DATAWRITER_QOS_DEFAULT,
                        null, /* listener */
                        StatusKind.STATUS_MASK_NONE);

            example0 instance = new example0();
            InstanceHandle_t instance_handle = InstanceHandle_t.HANDLE_NIL;

            for (int count = 0;
                    (sampleCount == 0) || (count < sampleCount);
                    ++count) {
                System.out.println("Writing example0, count " + count);
                instance.msg = "Hello World! (" + count + ")";

               // Here we are going to pause capturing for some samples.
               // The resulting pcap file will not contain them.
                if (count == 4 && !NetworkCapture.pause()) {
                    System.err.println("Error pausing network capture");
                } else if (count == 6 && !NetworkCapture.resume()) {
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
            if (!NetworkCapture.stop()) {
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
