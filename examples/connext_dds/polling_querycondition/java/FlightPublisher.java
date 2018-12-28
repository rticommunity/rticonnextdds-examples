/*******************************************************************************
 (c) 2005-2015 Copyright, Real-Time Innovations, Inc.  All rights reserved.
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

public class FlightPublisher {
    // -----------------------------------------------------------------------
    // Public Methods
    // -----------------------------------------------------------------------

    public static void main(String[] args) {
        // --- Get domain ID --- //
        int domainId = 0;
        if (args.length >= 1) {
            domainId = Integer.valueOf(args[0]).intValue();
        }

        // -- Get max loop count; 0 means infinite loop --- //
        int sampleCount = 0;
        if (args.length >= 2) {
            sampleCount = Integer.valueOf(args[1]).intValue();
        }

        /* Uncomment this to turn on additional logging
        Logger.get_instance().set_verbosity_by_category(
            LogCategory.NDDS_CONFIG_LOG_CATEGORY_API,
            LogVerbosity.NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL);
        */

        // --- Run --- //
        publisherMain(domainId, sampleCount);
    }

    private static void publisherMain(int domainId, int sampleCount) {
        DomainParticipant participant = null;
        try {
            // --- Create participant --- //
            participant = DomainParticipantFactory.TheParticipantFactory.
                create_participant(
                    domainId, DomainParticipantFactory.PARTICIPANT_QOS_DEFAULT,
                    null /* listener */, StatusKind.STATUS_MASK_NONE);
            if (participant == null) {
                System.err.println("create_participant error\n");
                return;
            }

            // --- Create publisher --- //
            Publisher publisher = participant.create_publisher(
                DomainParticipant.PUBLISHER_QOS_DEFAULT, null /* listener */,
                StatusKind.STATUS_MASK_NONE);
            if (publisher == null) {
                System.err.println("create_publisher error\n");
                return;
            }

            // --- Create topic --- //
            // Register type before creating topic
            String typeName = FlightTypeSupport.get_type_name();
            FlightTypeSupport.register_type(participant, typeName);

            Topic topic = participant.create_topic(
                "Example Flight",
                typeName, DomainParticipant.TOPIC_QOS_DEFAULT,
                null /* listener */, StatusKind.STATUS_MASK_NONE);
            if (topic == null) {
                System.err.println("create_topic error\n");
                return;
            }

            // --- Create writer --- //
            FlightDataWriter writer = (FlightDataWriter)
                publisher.create_datawriter(
                    topic, Publisher.DATAWRITER_QOS_DEFAULT,
                    null /* listener */, StatusKind.STATUS_MASK_NONE);
            if (writer == null) {
                System.err.println("create_datawriter error\n");
                return;
            }

            // --- Write --- //
            // Create data sample for writing.
            Flight[] flights_info = new Flight[4];
            flights_info[0] = new Flight();
            flights_info[0].trackId  = 1111;
            flights_info[0].company  = "CompanyA";
            flights_info[0].altitude = 15000;
            flights_info[1] = new Flight();
            flights_info[1].trackId  = 2222;
            flights_info[1].company  = "CompanyB";
            flights_info[1].altitude = 20000;
            flights_info[2] = new Flight();
            flights_info[2].trackId  = 3333;
            flights_info[2].company  = "CompanyA";
            flights_info[2].altitude = 30000;
            flights_info[3] = new Flight();
            flights_info[3].trackId  = 4444;
            flights_info[3].company  = "CompanyB";
            flights_info[3].altitude = 25000;

            final long sendPeriodMillis = 1000; // 1 second.
            for (int count = 0;
                (sampleCount == 0) || (count < sampleCount);
                count += flights_info.length) {

                // Update flight info latitude and write.
                System.out.println("Updating and sending values");
                for (int i = 0; i < flights_info.length; i++) {
                    // Set the plane altitude lineally (usually the max is
                    // at 41,000ft).
                    int altitude = flights_info[i].altitude + count * 100;
                    flights_info[i].altitude =
                        altitude >= 41000 ? 41000 : altitude;

                    System.out.format(
                        "\t[trackId: %d, company: %s, altitude: %d]\n",
                        flights_info[i].trackId, flights_info[i].company,
                        flights_info[i].altitude);

                    writer.write(flights_info[i], InstanceHandle_t.HANDLE_NIL);
                }

                try {
                    Thread.sleep(sendPeriodMillis);
                } catch (InterruptedException ix) {
                    System.err.println("INTERRUPTED");
                    break;
                }
            }

        } finally {
            // --- Shutdown --- //
            if(participant != null) {
                participant.delete_contained_entities();

                DomainParticipantFactory.TheParticipantFactory.
                    delete_participant(participant);
            }
            /* RTI Connext provides finalize_instance()
               method for people who want to release memory used by the
               participant factory singleton. Uncomment the following block of
               code for clean destruction of the participant factory
               singleton. */
            //DomainParticipantFactory.finalize_instance();
        }
    }
}
