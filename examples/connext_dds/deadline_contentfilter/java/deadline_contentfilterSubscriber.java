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

import java.text.DecimalFormat;
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
import com.rti.dds.subscription.DataReader;
import com.rti.dds.subscription.DataReaderAdapter;
import com.rti.dds.subscription.DataReaderListener;
import com.rti.dds.subscription.InstanceStateKind;
import com.rti.dds.subscription.RequestedDeadlineMissedStatus;
import com.rti.dds.subscription.SampleInfo;
import com.rti.dds.subscription.SampleInfoSeq;
import com.rti.dds.subscription.SampleStateKind;
import com.rti.dds.subscription.Subscriber;
import com.rti.dds.subscription.ViewStateKind;
import com.rti.dds.topic.ContentFilteredTopic;
import com.rti.dds.topic.Topic;

// ===========================================================================

public class deadline_contentfilterSubscriber
        extends Application implements AutoCloseable {
    private DomainParticipant participant = null;  // Usually one per
                                                   // application
    private deadline_contentfilterDataReader reader = null;
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

        // A Subscriber allows an application to create one or more DataReaders
        Subscriber subscriber =
                Objects.requireNonNull(participant.create_subscriber(
                        DomainParticipant.SUBSCRIBER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        // Register type before creating topic
        String typeName = deadline_contentfilterTypeSupport.get_type_name();
        deadline_contentfilterTypeSupport.register_type(participant, typeName);

        // Create a Topic with a name and a datatype
        Topic topic = Objects.requireNonNull(participant.create_topic(
                "Example deadline_contentfilter",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));

        //// Start changes for Deadline
        // Create content filtered topic

        StringSeq parameters = new StringSeq(1);
        parameters.add("2");

        ContentFilteredTopic cft = participant.create_contentfilteredtopic(
                "ContentFilteredTopic",
                topic,
                "code < %0",
                parameters);


        DataReaderListener listener = new deadline_contentfilterListener();
        reader = (deadline_contentfilterDataReader) Objects.requireNonNull(
                subscriber.create_datareader(
                        cft,
                        Subscriber.DATAREADER_QOS_DEFAULT,
                        listener,
                        StatusKind.STATUS_MASK_ALL));

        /*
        DataReaderQos datareader_qos = new DataReaderQos();
        subscriber.get_default_datareader_qos(datareader_qos);

        // Set deadline QoS
        datareader_qos.deadline.period.sec = 2;
        datareader_qos.deadline.period.nanosec = 0;
        */

        final long receivePeriodSec = 1;
        boolean filterUpdated = false;

        // Main loop. Wait for data to arrive and process when it arrives
        while (!isShutdownRequested() && samplesRead < getMaxSampleCount()) {
            if (samplesRead >= 5 && !filterUpdated) {
                System.out.println("Starting to filter out instance1");
                parameters.set(0, "1");
                cft.set_expression_parameters(parameters);
                filterUpdated = true;
            }

            try {
                Thread.sleep(receivePeriodSec * 1000);  // in millisec
            } catch (InterruptedException ix) {
                System.err.println("INTERRUPTED");
                break;
            }
        }
    }

    // -----------------------------------------------------------------------
    // Private Types
    // -----------------------------------------------------------------------

    // =======================================================================

    private static class deadline_contentfilterListener
            extends DataReaderAdapter {
        deadline_contentfilterSeq _dataSeq = new deadline_contentfilterSeq();
        SampleInfoSeq _infoSeq = new SampleInfoSeq();

        //// Start changes for Deadline
        long _initms = System.currentTimeMillis();
        DecimalFormat decFormat = new DecimalFormat("#.##");

        @Override public void on_data_available(DataReader reader)
        {
            deadline_contentfilterDataReader deadlineReader =
                    (deadline_contentfilterDataReader) reader;

            try {
                deadlineReader.take(
                        _dataSeq,
                        _infoSeq,
                        ResourceLimitsQosPolicy.LENGTH_UNLIMITED,
                        SampleStateKind.ANY_SAMPLE_STATE,
                        ViewStateKind.ANY_VIEW_STATE,
                        InstanceStateKind.ANY_INSTANCE_STATE);

                for (int i = 0; i < _dataSeq.size(); ++i) {
                    SampleInfo info = (SampleInfo) _infoSeq.get(i);

                    if (!info.valid_data) {
                        continue;
                    }
                    deadline_contentfilter data =
                            (deadline_contentfilter) _dataSeq.get(i);
                    double elapsed =
                            (System.currentTimeMillis() - _initms) / 1000.0;
                    System.out.print(
                            "@ t=" + decFormat.format(elapsed) + "s, Instance"
                            + data.code + ": <" + data.x + "," + data.y
                            + ">\n");
                    samplesRead++;
                }
            } catch (RETCODE_NO_DATA noData) {
                // No data to process
            } finally {
                deadlineReader.return_loan(_dataSeq, _infoSeq);
            }
        }

        @Override
        public void on_requested_deadline_missed(
                DataReader reader,
                RequestedDeadlineMissedStatus status)
        {
            deadline_contentfilterDataReader deadlineReader =
                    (deadline_contentfilterDataReader) reader;

            deadline_contentfilter dummy = new deadline_contentfilter();
            deadlineReader.get_key_value(dummy, status.last_instance_handle);

            double elapsed = (System.currentTimeMillis() - _initms) / 1000.0;
            System.out.print(
                    "Missed deadline @ t=" + decFormat.format(elapsed)
                    + "s on instance code = " + dummy.code + "\n");
        }
        //// End changes for Deadline
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
        try (deadline_contentfilterSubscriber subscriberApplication =
                     new deadline_contentfilterSubscriber()) {
            subscriberApplication.parseArguments(args);
            subscriberApplication.addShutdownHook();
            subscriberApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
