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

import com.rti.dds.domain.*;
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
import com.rti.dds.subscription.InstanceStateKind;
import com.rti.dds.subscription.ReadCondition;
import com.rti.dds.subscription.SampleInfo;
import com.rti.dds.subscription.SampleInfoSeq;
import com.rti.dds.subscription.SampleStateKind;
import com.rti.dds.subscription.Subscriber;
import com.rti.dds.subscription.ViewStateKind;
import com.rti.dds.topic.Topic;


public class waitset_statuscondSubscriber
        extends Application implements AutoCloseable {
    private DomainParticipant participant = null;  // Usually one per
                                                   // application
    private waitset_statuscondDataReader reader = null;
    private final waitset_statuscondSeq dataSeq = new waitset_statuscondSeq();
    private final SampleInfoSeq infoSeq = new SampleInfoSeq();

    private int processData(
            ConditionSeq activeConditions,
            StatusCondition statusCondition)
    {
        int samplesRead = 0;

        /* In this case, we have only a single condition, but
         * if we had multiple, we would need to iterate over
         * them and check which one is true.  Leaving the logic
         * for the more complex case.
         */
        for (int i = 0; i < activeConditions.size(); ++i) {
            /* Compare with Status Condition (not required as we
             * only have one condition, but leaving the logic for the
             * more complex case.)
             */
            if (activeConditions.get(i) == statusCondition) {
                /* Get the status changes so we can check which status
                 * condition triggered.
                 */
                int triggeredmask = reader.get_status_changes();

                // Data available
                if ((triggeredmask & StatusKind.DATA_AVAILABLE_STATUS) != 0) {
                    /* Current conditions match our conditions to read
                     * data, so we can read data just like we would do
                     * in any other example.
                     */

                    /* Access data using read(), take(), etc.  If you
                     * fail to do this the condition will remain true,
                     * and the WaitSet will wake up immediately -
                     * causing high CPU usage when it does not sleep in
                     * the loop
                     */
                    try {
                        reader.take(
                                dataSeq,
                                infoSeq,
                                ResourceLimitsQosPolicy.LENGTH_UNLIMITED,
                                SampleStateKind.ANY_SAMPLE_STATE,
                                ViewStateKind.ANY_VIEW_STATE,
                                InstanceStateKind.ANY_INSTANCE_STATE);

                        /* Print data */
                        for (int j = 0; j < dataSeq.size(); ++j) {
                            if (!((SampleInfo) infoSeq.get(j)).valid_data) {
                                System.out.println("Got metadata");
                                continue;
                            }
                            System.out.println("Received" + dataSeq.get(i));
                            samplesRead++;
                        }
                    } catch (RETCODE_NO_DATA noData) {
                        // Not data to process, not a problem
                    } finally {
                        /* Data loaned from Connext for performance.
                         *Return loan when done.
                         */
                        reader.return_loan(dataSeq, infoSeq);
                    }
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
        String typeName = waitset_statuscondTypeSupport.get_type_name();
        waitset_statuscondTypeSupport.register_type(participant, typeName);

        // Create a Topic with a name and a datatype
        Topic topic = Objects.requireNonNull(participant.create_topic(
                "Example waitset_statuscond",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));

        // This DataReader reads data on "Example waitset_statuscond" Topic
        reader = (waitset_statuscondDataReader) Objects.requireNonNull(
                subscriber.create_datareader(
                        topic,
                        Subscriber.DATAREADER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        /* Get status conditions
         * ---------------------
         * Each entity may have an attached Status Condition. To modify the
         * statuses we need to get the reader's Status Conditions first.
         */
        StatusCondition statusCondition =
                Objects.requireNonNull(reader.get_statuscondition());

        /* Set enabled statuses
         * --------------------
         * Now that we have the Status Condition, we are going to enable the
         * status we are interested in: knowing that data is available
         */
        statusCondition.set_enabled_statuses(StatusKind.DATA_AVAILABLE_STATUS);

        /* Create and attach conditions to the WaitSet
         * -------------------------------------------
         * Finally, we create the WaitSet and attach both the Read
         * Conditions and the Status Condition to it.
         */
        WaitSet waitset = new WaitSet();


        // Attach Status Conditions
        waitset.attach_condition(statusCondition);

        int samplesRead = 0;
        ConditionSeq activeConditions = new ConditionSeq();
        final Duration_t waitTimeout = new Duration_t(1, 500000000);

        // Main loop. Wait for data to arrive and process when it arrives
        while (!isShutdownRequested() && samplesRead < getMaxSampleCount()) {
            try {
                /* wait() blocks execution of the thread until one or more
                 * attached Conditions become true, or until a user-
                 * specified timeout expires.
                 */
                waitset.wait(activeConditions, waitTimeout);

                // Get the number of active conditions
                System.out.print(
                        "Got " + activeConditions.size()
                        + " active conditions\n");

                samplesRead += processData(activeConditions, statusCondition);

                // We get to timeout if no conditions were triggered
            } catch (RETCODE_TIMEOUT e) {
                System.out.println(
                        "Wait timed out!! No conditions were triggered.\n");
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
        try (waitset_statuscondSubscriber subscriberApplication =
                     new waitset_statuscondSubscriber()) {
            subscriberApplication.parseArguments(args);
            subscriberApplication.addShutdownHook();
            subscriberApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
