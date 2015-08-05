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
using System;
using System.Collections.Generic;
using System.Text;

namespace FlightExample
{
    public class FlightPublisher
    {
        public static void Main(string[] args)
        {
            // --- Get domain ID --- //
            int domainId = 0;
            if (args.Length >= 1) {
                domainId = Int32.Parse(args[0]);
            }

            // --- Get max loop count; 0 means infinite loop  --- //
            int sampleCount = 0;
            if (args.Length >= 2) {
                sampleCount = Int32.Parse(args[1]);
            }

            /* Uncomment this to turn on additional logging
            NDDS.ConfigLogger.get_instance().set_verbosity_by_category(
                NDDS.LogCategory.NDDS_CONFIG_LOG_CATEGORY_API,
                NDDS.LogVerbosity.NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL);
            */

            // --- Run --- //
            try {
                Publish(domainId, sampleCount);
            } catch(Exception) {
                Console.WriteLine("error in publisher");
            }
        }

        private static void Publish(int domainId, int sampleCount)
        {
            // --- Create participant --- //
            DDS.DomainParticipant participant =
                DDS.DomainParticipantFactory.get_instance().create_participant(
                    domainId,
                    DDS.DomainParticipantFactory.PARTICIPANT_QOS_DEFAULT,
                    null /* listener */,
                    DDS.StatusMask.STATUS_MASK_NONE);
            if (participant == null) {
                Shutdown(participant);
                throw new Exception("create_participant error");
            }

            // --- Create publisher --- //
            DDS.Publisher publisher = participant.create_publisher(
                DDS.DomainParticipant.PUBLISHER_QOS_DEFAULT,
                null /* listener */,
                DDS.StatusMask.STATUS_MASK_NONE);
            if (publisher == null) {
                Shutdown(participant);
                throw new Exception("create_publisher error");
            }

            // --- Create topic --- //
            // Register type before creating topic.
            string typeName = FlightTypeSupport.get_type_name();
            try {
                FlightTypeSupport.register_type(participant, typeName);
            } catch(DDS.Exception e) {
                Console.WriteLine("register_type error {0}", e);
                Shutdown(participant);
                throw e;
            }

            DDS.Topic topic = participant.create_topic(
                "Example Flight",
                typeName,
                DDS.DomainParticipant.TOPIC_QOS_DEFAULT,
                null /* listener */,
                DDS.StatusMask.STATUS_MASK_NONE);
            if (topic == null) {
                Shutdown(participant);
                throw new Exception("create_topic error");
            }

            // --- Create writer --- //
            DDS.DataWriter writer = publisher.create_datawriter(
                topic,
                DDS.Publisher.DATAWRITER_QOS_DEFAULT,
                null /* listener */,
                DDS.StatusMask.STATUS_MASK_NONE);
            if (writer == null) {
                Shutdown(participant);
                throw new Exception("create_datawriter error");
            }

            FlightDataWriter flightWriter = (FlightDataWriter)writer;

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

            // For a data type that has a key, if the same instance is going to
            // be written multiple times, initialize the key here and register
            // the keyed instance prior to writing.
            DDS.InstanceHandle_t instance_handle =
                DDS.InstanceHandle_t.HANDLE_NIL;

            // Main loop
            const int sendPeriod = 1000; // Milliseconds
            for (int count = 0;
                 (sampleCount == 0) || (count < sampleCount);
                 count++)
            {
                // Update flight info latitude and write.
                Console.WriteLine("Updating and sending values");

                for (int i = 0; i < flights_info.Length; i++) {
                    // Set the plane altitude lineally (usually the max is
                    // at 41,000ft).
                    int altitude = flights_info[i].altitude + count * 100;
                    flights_info[i].altitude =
                        altitude >= 41000 ? 41000 : altitude;

                    Console.WriteLine(
                        "\t[trackId: {0}, company: {1}, altitude: {2}]",
                        flights_info[i].trackId, flights_info[i].company,
                        flights_info[i].altitude);

                    try {
                        flightWriter.write(flights_info[i],ref instance_handle);
                    } catch (DDS.Exception e) {
                        Console.WriteLine("write error {0}", e);
                    }
                }

                System.Threading.Thread.Sleep(sendPeriod);
            }

            // Delete all entities
            Shutdown(participant);
        }

        private static void Shutdown(DDS.DomainParticipant participant)
        {
            // Delete all entities
            if (participant != null) {
                participant.delete_contained_entities();
                DDS.DomainParticipantFactory.get_instance().delete_participant(
                    ref participant);
            }

            // RTI Connext provides finalize_instance() method on
            // domain participant factory for people who want to release memory
            // used by the participant factory. Uncomment the following block of
            // code for clean destruction of the singleton. */
            /*
            try {
                DDS.DomainParticipantFactory.finalize_instance();
            } catch (DDS.Exception e) {
                Console.WriteLine("finalize_instance error: {0}", e);
                throw e;
            }
            */
        }
    }
}
