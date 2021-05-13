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
import com.rti.dds.subscription.*;
import com.rti.dds.topic.*;
import com.rti.ndds.config.*;

// ===========================================================================

public class NetworkCaptureSubscriber {
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

        subscriberMain(domainId, sampleCount);

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
    private NetworkCaptureSubscriber() {
        super();
    }

    // -----------------------------------------------------------------------

    private static void subscriberMain(int domainId, int sampleCount) {

        DomainParticipant participant = null;
        Subscriber subscriber = null;
        Topic topic = null;
        DataReaderListener listener = null;
        NetworkCaptureDataReader reader = null;

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
            if (!com.rti.ndds.utility.NetworkCapture.start("subscriber")) {
                System.err.println("Error starting network capture");
            }

            participant = DomainParticipantFactory.TheParticipantFactory.
                    create_participant(
                            domainId,
                            DomainParticipantFactory.PARTICIPANT_QOS_DEFAULT,
                            null, /* listener */
                            StatusKind.STATUS_MASK_NONE);
            if (participant == null) {
                System.err.println("create_participant error\n");
                return;
            }

            subscriber = participant.create_subscriber(
                DomainParticipant.SUBSCRIBER_QOS_DEFAULT, null /* listener */,
                StatusKind.STATUS_MASK_NONE);
            if (subscriber == null) {
                System.err.println("create_subscriber error\n");
                return;
            }

            String typeName = NetworkCaptureTypeSupport.get_type_name();
            NetworkCaptureTypeSupport.register_type(participant, typeName);

            topic = participant.create_topic(
                    "Network capture shared memory example",
                    typeName,
                    DomainParticipant.TOPIC_QOS_DEFAULT,
                    null, /* listener */
                    StatusKind.STATUS_MASK_NONE);
            if (topic == null) {
                System.err.println("create_topic error\n");
                return;
            }

            listener = new NetworkCaptureListener();

            reader = (NetworkCaptureDataReader) subscriber.create_datareader(
                    topic,
                    Subscriber.DATAREADER_QOS_DEFAULT,
                    listener,
                    StatusKind.STATUS_MASK_ALL);
            if (reader == null) {
                System.err.println("create_datareader error\n");
                return;
            }

            for (int count = 0;
                    (sampleCount == 0) || (count < sampleCount);
                    ++count) {
                System.out.println("NetworkCapture subscriber sleeping for 1 sec...");

                try {
                    Thread.sleep(1000);  // in millisec
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
        }
    }

    // -----------------------------------------------------------------------
    // Private Types
    // -----------------------------------------------------------------------

    // =======================================================================

    private static class NetworkCaptureListener extends DataReaderAdapter {

        NetworkCaptureSeq _dataSeq = new NetworkCaptureSeq();
        SampleInfoSeq _infoSeq = new SampleInfoSeq();

        public void on_data_available(DataReader reader) {
            NetworkCaptureDataReader NetworkCaptureReader =
            (NetworkCaptureDataReader)reader;

            try {
                NetworkCaptureReader.take(
                    _dataSeq, _infoSeq,
                    ResourceLimitsQosPolicy.LENGTH_UNLIMITED,
                    SampleStateKind.ANY_SAMPLE_STATE,
                    ViewStateKind.ANY_VIEW_STATE,
                    InstanceStateKind.ANY_INSTANCE_STATE);

                for(int i = 0; i < _dataSeq.size(); ++i) {
                    SampleInfo info = (SampleInfo)_infoSeq.get(i);

                    if (info.valid_data) {
                        System.out.println(
                            ((NetworkCapture)_dataSeq.get(i)).toString("Received",0));

                    }
                }
            } catch (RETCODE_NO_DATA noData) {
                // No data to process
            } finally {
                NetworkCaptureReader.return_loan(_dataSeq, _infoSeq);
            }
        }
    }
}

