/*
* (c) Copyright, Real-Time Innovations, 2020.  All rights reserved.
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

public class example0Publisher {
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
            example0Publisher.publish(domain_id, sample_count);
        } catch(DDS.Exception) {
            Console.WriteLine("error in publisher");
        }
    }

    static void publish(int domain_id, int sample_count) {
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
        if (!NDDS.NetworkCapture.start("publisher")) {
            throw new ApplicationException(
                    "Error starting network capture for all participants");
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

        DDS.Publisher publisher = participant.create_publisher(
                DDS.DomainParticipant.PUBLISHER_QOS_DEFAULT,
                null /* listener */,
                DDS.StatusMask.STATUS_MASK_NONE);
        if (publisher == null) {
            shutdown(participant);
            throw new ApplicationException("create_publisher error");
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

        DDS.DataWriter writer = publisher.create_datawriter(
                topic,
                DDS.Publisher.DATAWRITER_QOS_DEFAULT,
                null /* listener */,
                DDS.StatusMask.STATUS_MASK_NONE);
        if (writer == null) {
            shutdown(participant);
            throw new ApplicationException("create_datawriter error");
        }
        example0DataWriter example0_writer = (example0DataWriter) writer;

        example0 instance = example0TypeSupport.create_data();
        if (instance == null) {
            shutdown(participant);
            throw new ApplicationException(
                    "example0TypeSupport.create_data error");
        }

        DDS.InstanceHandle_t instance_handle = DDS.InstanceHandle_t.HANDLE_NIL;
        const System.Int32 send_period = 4000; // milliseconds
        for (int count=0; (sample_count == 0) || (count < sample_count); ++count) {
            Console.WriteLine("Writing example0, count {0}", count);

            /*
            * Here we are going to pause capturing for some samples.
            * The resulting pcap file will not contain them.
            */
            if (count == 4 && !NDDS.NetworkCapture.pause()) {
                shutdown(participant);
                throw new ApplicationException("Error pausing network capture");
            } else if (count == 6 && !NDDS.NetworkCapture.resume()) {
                shutdown(participant);
                throw new ApplicationException("Error resuming network capture");
            }

            try {
                instance.msg = "Hello World! (" + count + ")";
                example0_writer.write(instance, ref instance_handle);
            }
            catch(DDS.Exception e) {
                Console.WriteLine("write error {0}", e);
            }
            System.Threading.Thread.Sleep(send_period);
        }

        try {
            example0TypeSupport.delete_data(instance);
        } catch(DDS.Exception e) {
            Console.WriteLine("example0TypeSupport.delete_data error: {0}", e);
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
    }

    static void shutdown(DDS.DomainParticipant participant) {

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
        } catch (DDS.Exception e) {
            Console.WriteLine("finalize_instance error: {0}", e);
            throw e;
        }
    }
}