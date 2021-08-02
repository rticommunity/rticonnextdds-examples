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
import com.rti.dds.subscription.ReadCondition;
import com.rti.dds.subscription.SampleInfo;
import com.rti.dds.subscription.SampleInfoSeq;
import com.rti.dds.subscription.SampleStateKind;
import com.rti.dds.subscription.Subscriber;
import com.rti.dds.subscription.ViewStateKind;
import com.rti.dds.topic.ContentFilteredTopic;
import com.rti.dds.topic.Topic;

// ===========================================================================

public class cftSubscriber extends Application implements AutoCloseable {
    private DomainParticipant participant = null;  // Usually one per
                                                   // application
    private cftDataReader reader = null;
    private final cftSeq dataSeq = new cftSeq();
    private final SampleInfoSeq infoSeq = new SampleInfoSeq();

    private int processData()
    {
        int samplesRead = 0;

        try {
            // Take available data from DataReader's queue
            reader.take(
                    dataSeq,
                    infoSeq,
                    ResourceLimitsQosPolicy.LENGTH_UNLIMITED,
                    SampleStateKind.ANY_SAMPLE_STATE,
                    ViewStateKind.ANY_VIEW_STATE,
                    InstanceStateKind.ANY_INSTANCE_STATE);

            for (int i = 0; i < dataSeq.size(); ++i) {
                SampleInfo info = infoSeq.get(i);

                if (info.valid_data) {
                    System.out.println("Received" + dataSeq.get(i));
                }
                samplesRead++;
            }
        } catch (RETCODE_NO_DATA noData) {
            // No data to process, not a problem
        } finally {
            // Data loaned from Connext for performance. Return loan when done.
            reader.return_loan(dataSeq, infoSeq);
        }

        return samplesRead;
    }

    private void runApplication()
    {
        // Start communicating in a domain
        participant = Objects.requireNonNull(
                DomainParticipantFactory.get_instance().create_participant(
                        getDomainId(),
                        DomainParticipantFactory.PARTICIPANT_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        // --- Create subscriber --- //
        /* To customize subscriber QoS, use
           the configuration file USER_QOS_PROFILES.xml */

        // A Subscriber allows an application to create one or more DataReaders
        Subscriber subscriber =
                Objects.requireNonNull(participant.create_subscriber(
                        DomainParticipant.SUBSCRIBER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        // Register the datatype to use when creating the Topic
        String typeName = cftTypeSupport.get_type_name();
        cftTypeSupport.register_type(participant, typeName);

        // Create a Topic with a name and a datatype
        Topic topic = Objects.requireNonNull(participant.create_topic(
                "Example cft",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));

        // --- Create reader --- //

        // The default parameter list that we will include in the
        // sequence of parameters will be "1","4" (i.e., 1 <= x <= 4).
        String param_list[] = { "1", "4" };
        // Sequence of parameters for the content filter expression */
        StringSeq parameters =
                new StringSeq(java.util.Arrays.asList(param_list));

        /* Create the content filtered topic in case isCftSelected
         * is true.
         * The Content Filter Expresion has two parameters:
         * - %0 -- x must be greater or equal than %0.
         * - %1 -- x must be less or equal than %1.
         */
        ContentFilteredTopic cft = null;
        if (isCftSelected()) {
            cft = participant.create_contentfilteredtopic(
                    "ContentFilteredTopic",
                    topic,
                    "(x >= %0 and x <= %1)",
                    parameters);

            System.out.print("Using ContentFiltered Topic\n");
            reader = (cftDataReader) Objects.requireNonNull(
                    subscriber.create_datareader(
                            cft,
                            Subscriber.DATAREADER_QOS_DEFAULT,
                            null,
                            StatusKind.STATUS_MASK_ALL));
            System.out.print("\n==========================\n");
            System.out.print("Using CFT\nFilter: 1 <= x <= 4\n");
            System.out.print("===========================\n");
        } else {
            System.out.print("Using Normal Topic\n");
            reader = (cftDataReader) Objects.requireNonNull(
                    subscriber.create_datareader(
                            topic,
                            Subscriber.DATAREADER_QOS_DEFAULT,
                            null,
                            StatusKind.STATUS_MASK_ALL));
        }

        /* If you want to set the reliability and history QoS settings
         * programmatically rather than using the XML, you will need to add
         * the following lines to your code and comment out the
         * create_datareader calls above.
         */

        /*
        DataReaderQos datareader_qos = new DataReaderQos();
        subscriber.get_default_datareader_qos(datareader_qos);

        datareader_qos.reliability.kind =
            ReliabilityQosPolicyKind.RELIABLE_RELIABILITY_QOS;
        datareader_qos.durability.kind =
            DurabilityQosPolicyKind.TRANSIENT_LOCAL_DURABILITY_QOS;
        datareader_qos.history.kind =
            HistoryQosPolicyKind.KEEP_LAST_HISTORY_QOS;
        datareader_qos.history.depth = 20;

        if (isCftSelected()) {
            System.out.print("Using ContentFiltered Topic\n");
            reader = (cftDataReader) Objects.requireNonNull(
                subscriber.create_datareader(
                    cft, datareader_qos, listener,
                    StatusKind.STATUS_MASK_ALL));
            System.out.print("\n==========================\n");
            System.out.print("Using CFT\nFilter: 1 <= x <= 4\n");
            System.out.print("===========================\n");

        } else {
            System.out.print("Using Normal Topic\n");
            reader = (cftDataReader) Objects.requireNonNull(
                subscriber.create_datareader(
                    topic, datareader_qos, listener,
                    StatusKind.STATUS_MASK_ALL));
        }
        */

        // --- Wait for data --- //

        // Create ReadCondition that triggers when data in reader's queue
        ReadCondition condition = reader.create_readcondition(
                SampleStateKind.ANY_SAMPLE_STATE,
                ViewStateKind.ANY_VIEW_STATE,
                InstanceStateKind.ANY_INSTANCE_STATE);

        // WaitSet will be woken when the attached condition is triggered, or
        // timeout
        WaitSet waitset = new WaitSet();
        waitset.attach_condition(condition);
        final Duration_t waitTimeout = new Duration_t(1, 0);

        int samplesRead = 0;
        int iterationCount = 0;
        ConditionSeq activeConditions = new ConditionSeq();

        // Main loop. Wait for data to arrive and process when it arrives
        while (!isShutdownRequested() && samplesRead < getMaxSampleCount()) {
            /* If we are not using CFT, we do not need to change the filter
             * parameters */
            if (iterationCount == 10 && isCftSelected()) {
                System.out.print("\n==========================\n");
                System.out.print("Changing filter parameters\n");
                System.out.print("Filter: 5 <= x <= 9\n");
                System.out.print("===========================\n");

                parameters.set(0, "5");
                parameters.set(1, "9");

                cft.set_expression_parameters(parameters);

            } else if (iterationCount == 20 && isCftSelected()) {
                System.out.print("\n==========================\n");
                System.out.print("Changing filter parameters\n");
                System.out.print("Filter: 3 <= x <= 9\n");
                System.out.print("===========================\n");

                StringSeq oldParameters = new StringSeq();
                cft.get_expression_parameters(oldParameters);

                oldParameters.set(0, "3");
                cft.set_expression_parameters(oldParameters);
            }

            try {
                // Wait fills in activeConditions or times out
                waitset.wait(activeConditions, waitTimeout);

                // Read condition triggered, process data
                samplesRead += processData();

            } catch (RETCODE_TIMEOUT timeout) {
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
        try (cftSubscriber subscriberApplication = new cftSubscriber()) {
            subscriberApplication.parseArguments(
                    args,
                    ApplicationType.SUBSCRIBER);
            subscriberApplication.addShutdownHook();
            subscriberApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
