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
import com.rti.dds.infrastructure.RETCODE_ERROR;
import com.rti.dds.infrastructure.RETCODE_NO_DATA;
import com.rti.dds.infrastructure.RETCODE_TIMEOUT;
import com.rti.dds.infrastructure.ResourceLimitsQosPolicy;
import com.rti.dds.infrastructure.StatusKind;
import com.rti.dds.infrastructure.WaitSet;
import com.rti.dds.subscription.DataReader;
import com.rti.dds.subscription.DataReaderSeq;
import com.rti.dds.subscription.InstanceStateKind;
import com.rti.dds.subscription.ReadCondition;
import com.rti.dds.subscription.SampleInfo;
import com.rti.dds.subscription.SampleInfoSeq;
import com.rti.dds.subscription.SampleStateKind;
import com.rti.dds.subscription.Subscriber;
import com.rti.dds.subscription.SubscriberAdapter;
import com.rti.dds.subscription.ViewStateKind;
import com.rti.dds.topic.Topic;


public class ordered_groupSubscriber
        extends Application implements AutoCloseable {
    private DomainParticipant participant = null;  // Usually one per
                                                   // application
    private ordered_groupDataReader reader = null;
    private final ordered_groupSeq dataSeq = new ordered_groupSeq();
    private final SampleInfoSeq infoSeq = new SampleInfoSeq();

    private static int samplesRead = 0;

    private void runApplication()
    {
        // Start communicating in a domain
        participant = Objects.requireNonNull(
                DomainParticipantFactory.get_instance().create_participant(
                        getDomainId(),
                        DomainParticipantFactory.PARTICIPANT_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        // The listener has been modified

        // Start changes for Ordered Presentation Example
        /* Note that the StatusKind is DATA_ON_READERS_STATUS in order
         * to the incoming data can be read by the SubscriberListener
         */
        ordered_groupSubscriberListener listener =
                new ordered_groupSubscriberListener();
        Subscriber subscriber =
                Objects.requireNonNull(participant.create_subscriber(
                        DomainParticipant.SUBSCRIBER_QOS_DEFAULT,
                        listener,
                        StatusKind.DATA_ON_READERS_STATUS));

        // End changes for Ordered Presentation Example

        // Register type before creating topic
        String typeName = ordered_groupTypeSupport.get_type_name();
        ordered_groupTypeSupport.register_type(participant, typeName);

        // Start changes for Ordered Presentation Example

        // TOPICS

        Topic topic1 = Objects.requireNonNull(participant.create_topic(
                "Topic1",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));

        Topic topic2 = Objects.requireNonNull(participant.create_topic(
                "Topic2",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));

        Topic topic3 = Objects.requireNonNull(participant.create_topic(
                "Topic3",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));

        // DATAREADERS

        DataReader reader1 =
                Objects.requireNonNull(subscriber.create_datareader(
                        topic1,
                        Subscriber.DATAREADER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_ALL));

        DataReader reader2 =
                Objects.requireNonNull(subscriber.create_datareader(
                        topic2,
                        Subscriber.DATAREADER_QOS_DEFAULT,
                        null,  // lstener
                        StatusKind.STATUS_MASK_ALL));

        DataReader reader3 =
                Objects.requireNonNull(subscriber.create_datareader(
                        topic3,
                        Subscriber.DATAREADER_QOS_DEFAULT,
                        null,  // lstener
                        StatusKind.STATUS_MASK_ALL));

        // End changes for Ordered Presentation Example

        final long receivePeriodSec = 4;

        // Main loop. Wait for data to arrive and process when it arrives
        while (!isShutdownRequested() && samplesRead < getMaxSampleCount()) {
            System.out.println(
                    "ordered_group subscriber sleeping for " + receivePeriodSec
                    + " sec...");
            try {
                Thread.sleep(receivePeriodSec * 1000);  // in millisec
            } catch (InterruptedException ix) {
                System.err.println("INTERRUPTED");
                break;
            }
        }
    }

    // Start changes for Ordered Presentation Group example
    private static class ordered_groupSubscriberListener
            extends SubscriberAdapter {
        ordered_groupDataReader ordered_group_reader = null;
        ordered_group data = new ordered_group();
        SampleInfo info = new SampleInfo();
        DataReaderSeq my_datareaders = new DataReaderSeq();

        public void on_data_on_readers(Subscriber subs)
        {
            // IMPORTANT for GROUP access scope: Invoking begin_access()
            subs.begin_access();

            /* Obtain DataReaders. We obtain a sequence of DataReaders that
             * specifies the order in which each sample should be read
             */
            try {
                subs.get_datareaders(
                        my_datareaders,
                        SampleStateKind.ANY_SAMPLE_STATE,
                        ViewStateKind.ANY_VIEW_STATE,
                        InstanceStateKind.ANY_INSTANCE_STATE);
            } catch (RETCODE_ERROR error) {
                System.out.println("Error in get_datareaders: " + error);
                my_datareaders.setMaximum(0);
                subs.end_access();
                return;
            }

            // Read the samples received, following the DataReaders sequence
            try {
                for (int i = 0; i < my_datareaders.size(); ++i) {
                    try {
                        ordered_group_reader =
                                (ordered_groupDataReader) my_datareaders.get(i);
                    } catch (RETCODE_ERROR error) {
                        my_datareaders.setMaximum(0);
                        subs.end_access();
                        return;
                    }
                    /* IMPORTANT. Use take_next_sample(). We need to take only
                     * one sample each time, as we want to follow the sequence
                     * of DataReaders. This way the samples will be returned in
                     * the order in which they were modified
                     */

                    ordered_group_reader.take_next_sample(data, info);

                    if (info.valid_data) {
                        System.out.println(
                                ((ordered_group) data).toString("Received", 0));
                    }

                    samplesRead++;
                    my_datareaders.setMaximum(0);
                }

                // IMPORTANT for GROUP access scope: Invoking end_access()
                subs.end_access();
            } catch (RETCODE_NO_DATA noData) {
                // No data to process
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
        try (ordered_groupSubscriber subscriberApplication =
                     new ordered_groupSubscriber()) {
            subscriberApplication.parseArguments(args);
            subscriberApplication.addShutdownHook();
            subscriberApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
