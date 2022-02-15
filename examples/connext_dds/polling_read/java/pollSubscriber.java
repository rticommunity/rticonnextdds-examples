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
import com.rti.dds.infrastructure.WaitSet;
import com.rti.dds.subscription.InstanceStateKind;
import com.rti.dds.subscription.ReadCondition;
import com.rti.dds.subscription.SampleInfo;
import com.rti.dds.subscription.SampleInfoSeq;
import com.rti.dds.subscription.SampleStateKind;
import com.rti.dds.subscription.Subscriber;
import com.rti.dds.subscription.ViewStateKind;
import com.rti.dds.topic.Topic;


public class pollSubscriber extends Application implements AutoCloseable {
    private DomainParticipant participant = null;  // Usually one per
                                                   // application
    private pollDataReader reader = null;

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
        String typeName = pollTypeSupport.get_type_name();
        pollTypeSupport.register_type(participant, typeName);

        // Create a Topic with a name and a datatype
        Topic topic = Objects.requireNonNull(participant.create_topic(
                "Example poll",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));

        /* If you want to change datareader_qos.history.kind
         * programmatically rather than using the XML file, you will need
         * to add the following lines to your code and comment out the
         * create_datareader call above.
         */
        /*
        DataReaderQos datareader_qos = new DataReaderQos();
        subscriber.get_default_datareader_qos(datareader_qos);

        datareader_qos.history.kind =
            HistoryQosPolicyKind.KEEP_LAST_HISTORY_QOS;

        reader = (pollDataReader) Objects.requireNonNull(
                subscriber.create_datareader(
                        topic,
                        detareader_qos,
                        null,  // listener
                        StatusKind.STATUS_MASK_ALL));
        */

        // This DataReader reads data on "Example poll" Topic
        reader = (pollDataReader) Objects.requireNonNull(
                subscriber.create_datareader(
                        topic,
                        Subscriber.DATAREADER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_ALL));

        // Poll for new samples every 5 seconds
        final long receivePeriodSec = 5;
        int samplesRead = 0;
        pollSeq dataSeq = new pollSeq();
        SampleInfoSeq infoSeq = new SampleInfoSeq();

        // Main loop. Wait for data to arrive and process when it arrives
        while (!isShutdownRequested() && samplesRead < getMaxSampleCount()) {
            System.out.println(
                    "poll subscriber sleeping for " + receivePeriodSec
                    + " sec...");

            int length = 0;
            double sum = 0;

            try {
                reader.take(
                        dataSeq,
                        infoSeq,
                        ResourceLimitsQosPolicy.LENGTH_UNLIMITED,
                        SampleStateKind.ANY_SAMPLE_STATE,
                        ViewStateKind.ANY_VIEW_STATE,
                        InstanceStateKind.ANY_INSTANCE_STATE);

                for (int i = 0; i < dataSeq.size(); ++i) {
                    SampleInfo info = (SampleInfo) infoSeq.get(i);

                    if (!info.valid_data) {
                        continue;
                    }

                    length++;
                    sum = sum + ((poll) dataSeq.get(i)).x;

                    samplesRead++;
                }

                if (length > 0) {
                    System.out.println(
                            "Got " + length
                            + " samples. Avg = " + sum / length);
                }
            } catch (RETCODE_NO_DATA noData) {
                // No data to process
            } finally {
                reader.return_loan(dataSeq, infoSeq);
            }

            try {
                Thread.sleep(receivePeriodSec * 1000);  // in millisec
            } catch (InterruptedException ix) {
                System.err.println("INTERRUPTED");
                break;
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
        try (pollSubscriber subscriberApplication = new pollSubscriber()) {
            subscriberApplication.parseArguments(args);
            subscriberApplication.addShutdownHook();
            subscriberApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
