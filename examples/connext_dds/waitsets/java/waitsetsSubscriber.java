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
import com.rti.dds.infrastructure.StatusCondition;
import com.rti.dds.infrastructure.StatusKind;
import com.rti.dds.infrastructure.WaitSet;
import com.rti.dds.subscription.DataReaderQos;
import com.rti.dds.subscription.InstanceStateKind;
import com.rti.dds.subscription.LivelinessChangedStatus;
import com.rti.dds.subscription.ReadCondition;
import com.rti.dds.subscription.SampleInfo;
import com.rti.dds.subscription.SampleInfoSeq;
import com.rti.dds.subscription.SampleStateKind;
import com.rti.dds.subscription.Subscriber;
import com.rti.dds.subscription.SubscriptionMatchedStatus;
import com.rti.dds.subscription.ViewStateKind;
import com.rti.dds.topic.Topic;


public class waitsetsSubscriber extends Application implements AutoCloseable {
    private DomainParticipant participant = null;  // Usually one per
                                                   // application
    private waitsetsDataReader reader = null;
    private final waitsetsSeq dataSeq = new waitsetsSeq();
    private final SampleInfoSeq infoSeq = new SampleInfoSeq();

    private int process_data(
            StatusCondition statusCondition,
            ReadCondition readCondition,
            ConditionSeq activeConditions)
    {
        int samplesRead = 0;

        // Get the number of active conditions
        System.out.print(
                "Got " + activeConditions.size() + " active conditions\n");
        for (int i = 0; i < activeConditions.size(); ++i) {
            /* Now we compare the current condition with the Status
             * Conditions and the Read Conditions previously defined. If
             * they match, we print the condition that was triggered.*/

            /* Compare with Status Conditions */
            if (activeConditions.get(i) == statusCondition) {
                /* Get the status changes so we can check which status
                 * condition triggered. */
                int triggeredmask = reader.get_status_changes();

                // Liveliness changed
                if ((triggeredmask & StatusKind.LIVELINESS_CHANGED_STATUS)
                    != 0) {
                    LivelinessChangedStatus st = new LivelinessChangedStatus();
                    reader.get_liveliness_changed_status(st);
                    System.out.print(
                            "Liveliness changed => "
                            + "Active writers = " + st.alive_count + "\n");
                }

                // Subscription matched
                if ((triggeredmask & StatusKind.SUBSCRIPTION_MATCHED_STATUS)
                    != 0) {
                    SubscriptionMatchedStatus st =
                            new SubscriptionMatchedStatus();
                    reader.get_subscription_matched_status(st);
                    System.out.print(
                            "Subscription matched => "
                            + "Cumulative matches = " + st.total_count + "\n");
                }
            }
            // Compare with Read Conditions
            else if (activeConditions.get(i) == readCondition) {
                /* You may want to call take_w_condition() or
                 * read_w_condition() on the Data Reader. This way you
                 * will use the same status masks that were set on the
                 * Read Condition. This is just a suggestion, you can
                 * always use read() or take() like in any other
                 * example.
                 */
                try {
                    reader.take_w_condition(
                            dataSeq,
                            infoSeq,
                            ResourceLimitsQosPolicy.LENGTH_UNLIMITED,
                            readCondition);

                    for (int j = 0; j < infoSeq.size(); ++j) {
                        SampleInfo info = infoSeq.get(j);

                        if (!info.valid_data) {
                            System.out.print("Got metadata\n");
                            continue;
                        }

                        waitsets data = (waitsets) dataSeq.get(j);
                        System.out.println("   x: " + data.x);
                        samplesRead++;
                    }
                } catch (RETCODE_NO_DATA noData) {
                    // No data to process, not a problem
                } finally {
                    reader.return_loan(dataSeq, infoSeq);
                }
            }
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

        // A Subscriber allows an application to create one or more DataReaders
        Subscriber subscriber =
                Objects.requireNonNull(participant.create_subscriber(
                        DomainParticipant.SUBSCRIBER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        // Register type before creating topic
        String typeName = waitsetsTypeSupport.get_type_name();
        waitsetsTypeSupport.register_type(participant, typeName);

        // Create a Topic with a name and a datatype
        Topic topic = Objects.requireNonNull(participant.create_topic(
                "Example waitsets",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));

        // This DataReader reads data on "Example waitsets" Topic
        reader = (waitsetsDataReader) Objects.requireNonNull(
                subscriber.create_datareader(
                        topic,
                        Subscriber.DATAREADER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        /* If you want to change the DataReader's QoS programmatically
         * rather than using the XML file, you will need to add the
         * following lines to your code and comment out the
         * create_datareader call above.
         *
         * In this case, we reduce the liveliness timeout period to trigger
         * the StatusCondition DDS_LIVELINESS_CHANGED_STATUS
         */

        /*
        DataReaderQos datareader_qos = new DataReaderQos();
        subscriber.get_default_datareader_qos(datareader_qos);

        datareader_qos.liveliness.lease_duration.sec = 2;
        datareader_qos.liveliness.lease_duration.nanosec = 0;

        reader = (waitsetsDataReader) Objects.requireNonNull(
                subscriber.create_datareader(
                    topic, datareader_qos, null,
                    StatusKind.STATUS_MASK_NONE));
        */

        /* Create read condition
         * ---------------------
         * Note that the Read Conditions are dependent on both incoming
         * data as well as sample state. Thus, this method has more
         * overhead than adding a DDS_DATA_AVAILABLE_STATUS StatusCondition.
         * We show it here purely for reference
         */
        ReadCondition readCondition = reader.create_readcondition(
                SampleStateKind.NOT_READ_SAMPLE_STATE,
                ViewStateKind.ANY_VIEW_STATE,
                InstanceStateKind.ANY_INSTANCE_STATE);

        /* Get status conditions
         * ---------------------
         * Each entity may have an attached Status Condition. To modify the
         * statuses we need to get the reader's Status Conditions first.
         */
        StatusCondition statusCondition = reader.get_statuscondition();

        /* Set enabled statuses
         * --------------------
         * Now that we have the Status Condition, we are going to enable the
         * statuses we are interested in: DDS_SUBSCRIPTION_MATCHED_STATUS
         * and DDS_LIVELINESS_CHANGED_STATUS.
         */
        statusCondition.set_enabled_statuses(
                StatusKind.SUBSCRIPTION_MATCHED_STATUS
                | StatusKind.LIVELINESS_CHANGED_STATUS);

        /* Create and attach conditions to the WaitSet
         * -------------------------------------------
         * Finally, we create the WaitSet and attach both the Read
         * Conditions and the Status Condition to it.
         */
        WaitSet waitset = new WaitSet();
        waitset.attach_condition(readCondition);
        waitset.attach_condition(statusCondition);

        Duration_t waitTimeout = new Duration_t(1, 500000000);
        ConditionSeq activeConditions = new ConditionSeq();
        int samplesRead = 0;

        // Main loop. Wait for data to arrive and process when it arrives
        while (!isShutdownRequested() && samplesRead < getMaxSampleCount()) {
            try {
                /* wait() blocks execution of the thread until one or more
                 * attached Conditions become true, or until a
                 * user-specified timeout expires.
                 */
                waitset.wait(activeConditions, waitTimeout);

                samplesRead += process_data(
                        statusCondition,
                        readCondition,
                        activeConditions);
            } catch (RETCODE_TIMEOUT e) {
                // We get to timeout if no conditions were triggered
                System.out.print(
                        "Wait timed out!! No conditions were"
                        + " triggered\n");
                continue;
            }
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
        try (waitsetsSubscriber subscriberApplication =
                     new waitsetsSubscriber()) {
            subscriberApplication.parseArguments(args);
            subscriberApplication.addShutdownHook();
            subscriberApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
