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
    public class FlightSubscriber
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
                Subscribe(domainId, sampleCount);
            } catch(Exception ex) {
                Console.WriteLine("error in subscriber: {0}", ex);
            }
        }

        private static void Subscribe(int domainId, int sampleCount)
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

            // --- Create subscriber --- //
            DDS.Subscriber subscriber = participant.create_subscriber(
                DDS.DomainParticipant.SUBSCRIBER_QOS_DEFAULT,
                null /* listener */,
                DDS.StatusMask.STATUS_MASK_NONE);
            if (subscriber == null) {
                Shutdown(participant);
                throw new ApplicationException("create_subscriber error");
            }

            // --- Create topic --- //
            // Register the type before creating the topic.
            string typeName = FlightTypeSupport.get_type_name();
            try {
                FlightTypeSupport.register_type(participant, typeName);
            }
            catch(DDS.Exception e) {
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
                throw new ApplicationException("create_topic error");
            }

            // --- Create reader --- //
            DDS.DataReader reader = subscriber.create_datareader(
                topic,
                DDS.Subscriber.DATAREADER_QOS_DEFAULT,
                null,
                DDS.StatusMask.STATUS_MASK_ALL);
            if (reader == null) {
                Shutdown(participant);
                throw new ApplicationException("create_datareader error");
            }

            FlightDataReader flightReader = (FlightDataReader)reader;

            // Query for company named 'CompanyA' and for flights in cruise
            // (about 30,000ft). The company parameter will be changed in
            // run-time. NOTE: There must be single-quotes in the query
            // parameters around-any strings! The single-quote do NOT go in the
            // query condition itself.
            DDS.StringSeq queryParameters = new DDS.StringSeq();
            queryParameters.ensure_length(2, 2);
            queryParameters.set_at(0, "'CompanyA'");
            queryParameters.set_at(1, "30000");
            Console.WriteLine(
                "Setting parameters to company: {0} and altitude >= {1}\n",
                queryParameters.get_at(0), queryParameters.get_at(1));

            // Create the query condition with an expession to MATCH the id
            // field in the structure and a numeric comparison.
            DDS.QueryCondition queryCondition =
                reader.create_querycondition(
                    DDS.SampleStateKind.ANY_SAMPLE_STATE,
                    DDS.ViewStateKind.ANY_VIEW_STATE,
                    DDS.InstanceStateKind.ALIVE_INSTANCE_STATE,
                    "company MATCH %0 AND altitude >= %1",
                    queryParameters);

            // --- Wait for data --- //
            const int receivePeriod = 1000; // Milliseconds
            bool update = false;
            for (int count = 0;
                 (sampleCount == 0) || (count < sampleCount);
                 count++)
            {
                // Poll for new samples every second.
                System.Threading.Thread.Sleep(receivePeriod);

                // Change the filter parameter after 5 seconds.
                if ((count + 1) % 10 == 5) {
                    queryParameters.set_at(0, "'CompanyB'");
                    update = true;
                } else if ((count + 1) % 10 == 0) {
                    queryParameters.set_at(0, "'CompanyA'");
                    update = true;
                }

                // Set new parameters.
                if (update) {
                    Console.WriteLine("Changing parameter to {0}",
                        queryParameters.get_at(0));
                    queryCondition.set_query_parameters(queryParameters);
                    update = false;
                }

                // Iterate through the samples using read_w_condition.
                FlightSeq dataSeq = new FlightSeq();
                DDS.SampleInfoSeq infoSeq = new DDS.SampleInfoSeq();
                try {
                    flightReader.read_w_condition(
                        dataSeq,
                        infoSeq,
                        DDS.ResourceLimitsQosPolicy.LENGTH_UNLIMITED,
                        queryCondition);
                } catch (DDS.Retcode_NoData) {
                    continue;
                } catch (DDS.Exception e) {
                    Shutdown(participant);
                    throw e;
                }

                for (int i = 0; i < dataSeq.length; i++) {
                    DDS.SampleInfo info = (DDS.SampleInfo)infoSeq.get_at(i);
                    if (info.valid_data) {
                        Flight flight_info = (Flight)dataSeq.get_at(i);
                        Console.WriteLine(
                            "\t[trackId: {0}, company: {1}, altitude: {2}]\n",
                            flight_info.trackId,
                            flight_info.company,
                            flight_info.altitude);
                    }
                }

                try {
                    flightReader.return_loan(dataSeq, infoSeq);
                } catch (DDS.Exception e) {
                    Console.WriteLine("return loan error {0}", e);
                }
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
            // domain participant factory for users who want to release memory
            // used by the participant factory. Uncomment the following block of
            // code for clean destruction of the singleton.
            /*
            try {
                DDS.DomainParticipantFactory.finalize_instance();
            }
            catch(DDS.Exception e) {
                Console.WriteLine("finalize_instance error {0}", e);
                throw e;
            }
            */
        }
    }
}
