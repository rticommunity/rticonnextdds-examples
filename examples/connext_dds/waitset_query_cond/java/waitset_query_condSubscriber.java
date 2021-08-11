/*******************************************************************************
 (c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
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


public class waitset_query_condSubscriber
        extends Application implements AutoCloseable {
    private DomainParticipant participant = null;  // Usually one per
                                                   // application
    private waitset_query_condDataReader reader = null;
    private final waitset_query_condSeq dataSeq = new waitset_query_condSeq();
    private final SampleInfoSeq infoSeq = new SampleInfoSeq();

    private int processData(QueryCondition queryCondition)
    {
        int samplesRead = 0;
        try {
            reader.take_w_condition(
                    dataSeq,
                    infoSeq,
                    ResourceLimitsQosPolicy.LENGTH_UNLIMITED,
                    queryCondition);

            // Print data
            for (int i = 0; i < dataSeq.size(); ++i) {
                SampleInfo info = infoSeq.get(i);

                if (!info.valid_data) {
                    System.out.println("Got metadata");
                    continue;
                }
                System.out.println("Received" + dataSeq.get(i));
                samplesRead++;
            }
        } catch (RETCODE_NO_DATA noData) {
            // No data to process, not a problem
        } finally {
            // Return the loaned data
            reader.return_loan(dataSeq, infoSeq);
        }

        return samplesRead;
    }

    private void runApplication()
    {
        String even_string = new String("'EVEN'");
        String odd_string = new String("'ODD'");

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

        // Register type before creating topic
        String typeName = waitset_query_condTypeSupport.get_type_name();
        waitset_query_condTypeSupport.register_type(participant, typeName);

        // Create a Topic with a name and a datatype
        Topic topic = Objects.requireNonNull(participant.create_topic(
                "Example waitset_query_cond",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));

        // This DataReader reads data on "Example waitset_query_cond" Topic
        reader = (waitset_query_condDataReader) Objects.requireNonNull(
                subscriber.create_datareader(
                        topic,
                        Subscriber.DATAREADER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        // Create query condition
        StringSeq query_parameters = new StringSeq(1);
        query_parameters.add(even_string);
        String query_expression = new String("name MATCH %0");

        // The initial value of the parameters is EVEN string
        QueryCondition queryCondition =
                Objects.requireNonNull(reader.create_querycondition(
                        SampleStateKind.NOT_READ_SAMPLE_STATE,
                        ViewStateKind.ANY_VIEW_STATE,
                        InstanceStateKind.ANY_INSTANCE_STATE,
                        query_expression,
                        query_parameters));

        WaitSet waitset = new WaitSet();

        // Attach Query Conditions
        waitset.attach_condition(queryCondition);

        // wait_timeaut is 1.5 secs
        final Duration_t waitTimeout = new Duration_t(1, 500000000);

        System.out.println(
                "\n>>>Timeout: " + waitTimeout.sec + " sec & "
                + waitTimeout.nanosec + " nanosec");
        System.out.println(">>> Query conditions: name MATCH %0");
        System.out.println("\t%0 = " + query_parameters.get(0));
        System.out.println("---------------------------------\n");

        int samplesRead = 0;
        int iterationCount = 0;
        ConditionSeq activeConditions = new ConditionSeq();

        // Main loop. Wait for data to arrive and process when it arrives
        while (!isShutdownRequested() && samplesRead < getMaxSampleCount()) {
            // We set a new parameter in the Query Condition after 7 secs
            if (iterationCount == 7) {
                query_parameters.set(0, odd_string);
                System.out.println("CHANGING THE QUERY CONDITION");
                System.out.println("\n>>> Query conditions: name MATCH %0");
                System.out.println("\t%0 = " + query_parameters.get(0));
                System.out.println(
                        ">>> We keep one sample in the "
                        + "history");
                System.out.println(
                        "-------------------------------------"
                        + "\n");
                queryCondition.set_query_parameters(query_parameters);
            }

            /* wait() blocks execution of the thread until one or more
             * attached Conditions become true, or until a user-specified
             * timeout expires.
             */
            try {
                // Wait fills in activeConditions or times out
                waitset.wait(activeConditions, waitTimeout);

                // Read condition triggered, process data
                samplesRead += processData(queryCondition);

            } catch (RETCODE_TIMEOUT timeout) {
                // No data received, not a problem
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
        try (waitset_query_condSubscriber subscriberApplication =
                     new waitset_query_condSubscriber()) {
            subscriberApplication.parseArguments(args);
            subscriberApplication.addShutdownHook();
            subscriberApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
