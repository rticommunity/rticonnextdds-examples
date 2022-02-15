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

import java.util.Objects;

import com.rti.dds.domain.DomainParticipant;
import com.rti.dds.domain.DomainParticipantFactory;
import com.rti.dds.infrastructure.ConditionSeq;
import com.rti.dds.infrastructure.Duration_t;
import com.rti.dds.infrastructure.RETCODE_NO_DATA;
import com.rti.dds.infrastructure.RETCODE_TIMEOUT;
import com.rti.dds.infrastructure.ResourceLimitsQosPolicy;
import com.rti.dds.infrastructure.StatusKind;
import com.rti.dds.infrastructure.StringSeq;
import com.rti.dds.infrastructure.WaitSet;
import com.rti.dds.subscription.InstanceStateKind;
import com.rti.dds.subscription.QueryCondition;
import com.rti.dds.subscription.ReadCondition;
import com.rti.dds.subscription.SampleInfo;
import com.rti.dds.subscription.SampleInfoSeq;
import com.rti.dds.subscription.SampleStateKind;
import com.rti.dds.subscription.Subscriber;
import com.rti.dds.subscription.ViewStateKind;
import com.rti.dds.topic.Topic;

public class FlightSubscriber extends Application implements AutoCloseable {
    private DomainParticipant participant = null;  // Usually one per
                                                   // application
    private FlightDataReader reader = null;

    private void runApplication()
    {
        // Start communicating in a domain
        participant = Objects.requireNonNull(
                DomainParticipantFactory.get_instance().create_participant(
                        getDomainId(),
                        DomainParticipantFactory.PARTICIPANT_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        // A Subscriber allows an application to create one or more DataReaders
        Subscriber subscriber =
                Objects.requireNonNull(participant.create_subscriber(
                        DomainParticipant.SUBSCRIBER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        // Register type before creating topic.
        String typeName = FlightTypeSupport.get_type_name();
        FlightTypeSupport.register_type(participant, typeName);

        // Create a Topic with a name and a datatype
        Topic topic = Objects.requireNonNull(participant.create_topic(
                "Example Flight",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));

        // This DataReader reads data on "Example Flight" Topic
        reader = (FlightDataReader) Objects.requireNonNull(
                subscriber.create_datareader(
                        topic,
                        Subscriber.DATAREADER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_ALL));

        // Query for company named 'CompanyA' and for flights in cruise
        // (about 30,000ft). The company parameter will be changed in
        // run-time. NOTE: There must be single-quotes in the query
        // parameters around-any strings! The single-quote do NOT go in the
        // query condition itself.
        StringSeq queryParameters = new StringSeq(2);
        queryParameters.add("'CompanyA'");
        queryParameters.add("30000");
        System.out.format(
                "Setting parameters to company: %s and altitude >= %s\n",
                queryParameters.get(0),
                queryParameters.get(1));

        // Create the query condition with an expession to MATCH the id
        // field in the structure and a numeric comparison.
        QueryCondition queryCondition = reader.create_querycondition(
                SampleStateKind.ANY_SAMPLE_STATE,
                ViewStateKind.ANY_VIEW_STATE,
                InstanceStateKind.ALIVE_INSTANCE_STATE,
                "company MATCH %0 AND altitude >= %1",
                queryParameters);

        // --- Wait for data --- //
        final long receivePeriodSec = 1;
        Boolean update = false;
        int samplesRead = 0;
        int iterationCount = 0;

        // Main loop. Wait for data to arrive and process when it arrives
        while (!isShutdownRequested() && samplesRead < getMaxSampleCount()) {
            // Poll for new samples every second.
            try {
                Thread.sleep(receivePeriodSec * 1000);  // in millisec
            } catch (InterruptedException ix) {
                System.err.println("INTERRUPTED");
                break;
            }

            // Change the filter parameter after 5 seconds.
            if ((iterationCount + 1) % 10 == 5) {
                queryParameters.set(0, "'CompanyB'");
                update = true;
            } else if ((iterationCount + 1) % 10 == 0) {
                queryParameters.set(0, "'CompanyA'");
                update = true;
            }

            // Set new parameters.
            if (update) {
                System.out.println(
                        "Changing parameter to " + queryParameters.get(0));
                queryCondition.set_query_parameters(queryParameters);
                update = false;
            }

            // Iterate through the samples using read_w_condition.
            SampleInfoSeq infoSeq = new SampleInfoSeq();
            FlightSeq dataSeq = new FlightSeq();
            try {
                reader.read_w_condition(
                        dataSeq,
                        infoSeq,
                        ResourceLimitsQosPolicy.LENGTH_UNLIMITED,
                        queryCondition);

                for (int i = 0; i < dataSeq.size(); i++) {
                    SampleInfo info = (SampleInfo) infoSeq.get(i);
                    if (info.valid_data) {
                        Flight flight_info = (Flight) dataSeq.get(i);
                        System.out.format(
                                "\t[trackId: %d, company: %s, altitude: %d]\n",
                                flight_info.trackId,
                                flight_info.company,
                                flight_info.altitude);
                    }

                    samplesRead++;
                }

                reader.return_loan(dataSeq, infoSeq);
            } catch (RETCODE_NO_DATA noData) {
            }

            iterationCount++;
        }
    }

    @Override public void close()
    {
        // Delete all entities (DataReader, Topic, Subscriber,
        // DomainParticipant)
        if (participant != null) {
            participant.delete_contained_entities();

            DomainParticipantFactory.get_instance().delete_participant(
                    participant);
        }
    }

    public static void main(String[] args)
    {
        // Create example and run: Uses try-with-resources,
        // subscriberApplication.close() automatically called
        try (FlightSubscriber subscriberApplication = new FlightSubscriber()) {
            subscriberApplication.parseArguments(args);
            subscriberApplication.addShutdownHook();
            subscriberApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
