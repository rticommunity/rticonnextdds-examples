/*
* (c) Copyright, Real-Time Innovations, 2012.  All rights reserved.
* RTI grants Licensee a license to use, modify, compile, and create derivative
* works of the software solely for use with RTI Connext DDS. Licensee may
* redistribute copies of the software provided that all such copies are subject
* to this license. The software is provided "as is", with no warranty of any
* type, including any warranty for fitness for any purpose. RTI is under no
* obligation to maintain or support the software. RTI shall not be liable for
* any incidental or consequential damages arising out of the use or inability
* to use the software.
*/

/*
 * A simple HelloWorld using network capture to save DomainParticipant traffic.
 *
 * This example is a simple hello world running network capture for both a
 * publisher participant (example0_publisher.cs).and a subscriber participant
 * (example0_subscriber.cs).
 * It shows the basic flow when working with network capture:
 *   - Enabling before anything else.
 *   - Start capturing traffic (for one or all participants).
 *   - Pause/resume capturing traffic (for one or all participants).
 *   - Stop capturing trafffic (for one or all participants).
 *   - Disable after everything else.
 */

using System;
using System.Collections.Generic;
using System.Text;

public class example0Subscriber {

    public class example0Listener : DDS.DataReaderListener {
        public override void on_requested_deadline_missed(
            DDS.DataReader reader,
            ref DDS.RequestedDeadlineMissedStatus status) {}
        public override void on_requested_incompatible_qos(
            DDS.DataReader reader,
            DDS.RequestedIncompatibleQosStatus status) {}
        public override void on_sample_rejected(
            DDS.DataReader reader,
            ref DDS.SampleRejectedStatus status) {}
        public override void on_liveliness_changed(
            DDS.DataReader reader,
            ref DDS.LivelinessChangedStatus status) {}
        public override void on_sample_lost(
            DDS.DataReader reader,
            ref DDS.SampleLostStatus status) {}
        public override void on_subscription_matched(
            DDS.DataReader reader,
            ref DDS.SubscriptionMatchedStatus status) {}
        public override void on_data_available(DDS.DataReader reader) {
            example0DataReader example0_reader = (example0DataReader) reader;

            try {
                example0_reader.take(
                        data_seq,
                        info_seq,
                        DDS.ResourceLimitsQosPolicy.LENGTH_UNLIMITED,
                        DDS.SampleStateKind.ANY_SAMPLE_STATE,
                        DDS.ViewStateKind.ANY_VIEW_STATE,
                        DDS.InstanceStateKind.ANY_INSTANCE_STATE);
            }
            catch(DDS.Retcode_NoData) {
                return;
            }
            catch(DDS.Exception e) {
                Console.WriteLine("take error {0}", e);
                return;
            }

            System.Int32 data_length = data_seq.length;
            for (int i = 0; i < data_length; ++i) {
                if (info_seq.get_at(i).valid_data) {
                    example0TypeSupport.print_data(data_seq.get_at(i));
                }
            }

            try {
                example0_reader.return_loan(data_seq, info_seq);
            }
            catch(DDS.Exception e) {
                Console.WriteLine("return loan error {0}", e);
            }
        }

        public example0Listener() {
            data_seq = new example0Seq();
            info_seq = new DDS.SampleInfoSeq();
        }

        private example0Seq data_seq;
        private DDS.SampleInfoSeq info_seq;
    };

    public static void Main(string[] args) {

        int domain_id = 0;
        if (args.Length >= 1) {
            domain_id = Int32.Parse(args[0]);
        }

        int sample_count = 0;
        if (args.Length >= 2) {
            sample_count = Int32.Parse(args[1]);
        }

        try {
            example0Subscriber.subscribe(domain_id, sample_count);
        }
        catch(DDS.Exception) {
            Console.WriteLine("error in subscriber");
        }
    }

    static void subscribe(int domain_id, int sample_count) {

        /*
         * Enable network capture.
         *
         * This must be called before:
         *   - Any other network capture function is called.
         *   - Creating the participants for which we want to capture traffic.
         */
        if (!NDDS.NetworkCapture.enable()) {
            throw new ApplicationException("Error enabling network capture");
        }

        /*
         * Start capturing traffic for all participants.
         *
         * All participants: those already created and those yet to be created.
         * Default parameters: all transports and some other sane defaults.
         *
         * A capture file will be created for each participant. The capture file
         * will start with the prefix "publisher" and continue with a suffix
         * dependent on the participant's GUID.
         */
        if (!NDDS.NetworkCapture.start("subscriber")) {
            throw new ApplicationException("Error starting network capture for all participants");
        }

        DDS.DomainParticipant participant =
        DDS.DomainParticipantFactory.get_instance().create_participant(
                domain_id,
                DDS.DomainParticipantFactory.PARTICIPANT_QOS_DEFAULT,
                null /* listener */,
                DDS.StatusMask.STATUS_MASK_NONE);
        if (participant == null) {
            shutdown(participant);
            throw new ApplicationException("create_participant error");
        }

        DDS.Subscriber subscriber = participant.create_subscriber(
                DDS.DomainParticipant.SUBSCRIBER_QOS_DEFAULT,
                null /* listener */,
                DDS.StatusMask.STATUS_MASK_NONE);
        if (subscriber == null) {
            shutdown(participant);
            throw new ApplicationException("create_subscriber error");
        }

        System.String type_name = example0TypeSupport.get_type_name();
        try {
            example0TypeSupport.register_type(participant, type_name);
        }
        catch(DDS.Exception e) {
            Console.WriteLine("register_type error {0}", e);
            shutdown(participant);
            throw e;
        }

        DDS.Topic topic = participant.create_topic(
                "Example example0",
                type_name,
                DDS.DomainParticipant.TOPIC_QOS_DEFAULT,
                null /* listener */,
                DDS.StatusMask.STATUS_MASK_NONE);
        if (topic == null) {
            shutdown(participant);
            throw new ApplicationException("create_topic error");
        }

        example0Listener reader_listener = new example0Listener();

        DDS.DataReader reader = subscriber.create_datareader(
                topic,
                DDS.Subscriber.DATAREADER_QOS_DEFAULT,
                reader_listener,
                DDS.StatusMask.STATUS_MASK_ALL);
        if (reader == null) {
            shutdown(participant);
            reader_listener = null;
            throw new ApplicationException("create_datareader error");
        }

        const System.Int32 receive_period = 4000; // milliseconds
        for (int count=0; (sample_count == 0) || (count < sample_count); ++count) {
            Console.WriteLine(
                    "example0 subscriber sleeping for {0} sec...",
                    receive_period / 1000);

            System.Threading.Thread.Sleep(receive_period);
        }

        /*
        * Before deleting the participants that are capturing, we must stop
        * network capture for them.
        */
        if (!NDDS.NetworkCapture.stop()) {
            shutdown(participant);
            throw new ApplicationException("Error stopping network capture");
        }
        shutdown(participant);
        reader_listener = null;
    }

    static void shutdown(
        DDS.DomainParticipant participant) {

        if (participant != null) {
            participant.delete_contained_entities();
            DDS.DomainParticipantFactory.get_instance().delete_participant(
                ref participant);
        }

        /*
         * Disable network capture.
         *
         * This must be:
         *   - The last network capture function that is called.
         */
        if (!NDDS.NetworkCapture.disable()) {
            throw new ApplicationException("Error disabling network capture");
        }
        try {
            DDS.DomainParticipantFactory.finalize_instance();
        } catch(DDS.Exception e) {
            Console.WriteLine("finalize_instance error {0}", e);
            throw e;
        }
    }
}

