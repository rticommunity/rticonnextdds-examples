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
import com.rti.dds.domain.DomainParticipantAdapter;
import com.rti.dds.domain.DomainParticipantFactory;
import com.rti.dds.infrastructure.ConditionSeq;
import com.rti.dds.infrastructure.Duration_t;
import com.rti.dds.infrastructure.RETCODE_NO_DATA;
import com.rti.dds.infrastructure.RETCODE_TIMEOUT;
import com.rti.dds.infrastructure.ResourceLimitsQosPolicy;
import com.rti.dds.infrastructure.StatusKind;
import com.rti.dds.infrastructure.WaitSet;
import com.rti.dds.subscription.*;
import com.rti.dds.topic.InconsistentTopicStatus;
import com.rti.dds.topic.Topic;


public class listenersSubscriber extends Application implements AutoCloseable {
    private DomainParticipant participant = null;  // Usually one per
                                                   // application
    private static int samplesRead = 0;

    private void runApplication()
    {
        ParticipantListener participant_listener = new ParticipantListener();

        /* We associate the participant_listener to the participant and set the
         * status mask to get all the statuses
         */
        participant = Objects.requireNonNull(
                DomainParticipantFactory.get_instance().create_participant(
                        getDomainId(),
                        DomainParticipantFactory.PARTICIPANT_QOS_DEFAULT,
                        participant_listener /* listener */,
                        StatusKind.STATUS_MASK_ALL /* get all statuses */));

        SubscriberListener subsriber_listener = new SubscriberListener();

        /* Here we associate the subscriber listener to the subscriber and set
         * the status mask to get all the statuses
         */
        Subscriber subscriber =
                Objects.requireNonNull(participant.create_subscriber(
                        DomainParticipant.SUBSCRIBER_QOS_DEFAULT,
                        subsriber_listener,  // listener
                        StatusKind.STATUS_MASK_ALL));

        // Register type before creating topic
        String typeName = listenersTypeSupport.get_type_name();
        listenersTypeSupport.register_type(participant, typeName);

        // Create a Topic with a name and a datatype
        Topic topic = Objects.requireNonNull(participant.create_topic(
                "Example listeners",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));

        ReaderListener reader_listener = new ReaderListener();

        /* Here we associate the data reader listener to the reader.
         * We just listen for liveliness changed and data available,
         * since most specific listeners will get called
         */
        listenersDataReader reader =
                (listenersDataReader)
                        Objects.requireNonNull(
                                subscriber.create_datareader(
                                        topic,
                                        Subscriber.DATAREADER_QOS_DEFAULT,
                                        reader_listener /* listener */,
                                        StatusKind.LIVELINESS_CHANGED_STATUS
                                                | StatusKind
                                                          .DATA_AVAILABLE_STATUS /* statuses */));

        final long receivePeriodSec = 1;

        // Main loop. Wait for data to arrive and process when it arrives
        while (!isShutdownRequested() && samplesRead < getMaxSampleCount()) {
            try {
                Thread.sleep(receivePeriodSec * 1000);  // in millisec
            } catch (InterruptedException ix) {
                System.err.println("INTERRUPTED");
                break;
            }
        }
    }

    private static class ParticipantListener extends DomainParticipantAdapter {
        public void on_requested_deadline_missed(
                DataReader dataReader,
                RequestedDeadlineMissedStatus status)
        {
            System.out.println(
                    "ParticipantListener: on_requested_deadline_missed()");
        }

        public void on_requested_incompatible_qos(
                DataReader dataReader,
                RequestedIncompatibleQosStatus status)
        {
            System.out.println(
                    "ParticipantListener: on_requested_incompatible_qos()");
        }

        public void on_sample_rejected(
                DataReader dataReader,
                SampleRejectedStatus status)
        {
            System.out.println("ParticipantListener: on_sample_rejected()");
        }

        public void on_liveliness_changed(
                DataReader dataReader,
                LivelinessChangedStatus status)
        {
            System.out.println("ParticipantListener: on_liveliness_changed()");
        }

        public void on_sample_lost(
                DataReader dataReader,
                SampleLostStatus status)
        {
            System.out.println("ParticipantListener: on_sample_lost()");
        }

        public void on_subscription_matched(
                DataReader dataReader,
                SubscriptionMatchedStatus status)
        {
            System.out.println(
                    "ParticipantListener: on_subscription_matched()");
        }

        public void on_data_available(DataReader dataReader)
        {
            System.out.println("ParticipantListener: on_data_available()");
        }

        public void on_data_on_readers(Subscriber subscriber)
        {
            System.out.println("ParticipantListener: on_data_on_readers()");

            // notify_datareaders() only calls on_data_available for
            // DataReaders with unread samples
            subscriber.notify_datareaders();
        }

        public void on_inconsistent_topic(
                Topic topic,
                InconsistentTopicStatus status)
        {
            System.out.println("ParticipantListener: on_inconsistent_topic()");
        }
    }

    private static class SubscriberListener extends SubscriberAdapter {
        public void on_requested_deadline_missed(
                DataReader dataReader,
                RequestedDeadlineMissedStatus status)
        {
            System.out.println(
                    "SubscriberListener: on_requested_deadline_missed()");
        }

        public void on_requested_incompatible_qos(
                DataReader dataReader,
                RequestedIncompatibleQosStatus status)
        {
            System.out.println(
                    "SubscriberListener: on_requested_incompatible_qos()");
        }

        public void on_sample_rejected(
                DataReader dataReader,
                SampleRejectedStatus status)
        {
            System.out.println("SubscriberListener: on_sample_rejected()");
        }

        public void on_liveliness_changed(
                DataReader dataReader,
                LivelinessChangedStatus status)
        {
            System.out.println("SubscriberListener: on_liveliness_changed()");
        }

        public void on_subscription_matched(
                DataReader dataReader,
                SubscriptionMatchedStatus status)
        {
            System.out.println("SubscriberListener: on_subscription_matched()");
        }

        public void on_data_available(DataReader dataReader)
        {
            System.out.println("SubscriberListener: on_data_available()");
        }

        private int count = 0;

        public void on_data_on_readers(Subscriber subscriber)
        {
            System.out.println("SubscriberListener: on_data_on_readers()");

            // notify_datareaders() only calls on_data_available for
            // DataReaders with unread samples
            subscriber.notify_datareaders();

            if (++count > 3) {
                int newmask = StatusKind.STATUS_MASK_ALL;
                // 'Unmask' DATA_ON_READERS status for listener
                newmask &= ~StatusKind.DATA_ON_READERS_STATUS;
                subscriber.set_listener(this, newmask);
                System.out.print(
                        "Unregistering SubscriberListener::on_data_on_readers()\n");
            }
        }
    }


    private static class ReaderListener extends DataReaderAdapter {
        public void on_requested_deadline_missed(
                DataReader dataReader,
                RequestedDeadlineMissedStatus status)
        {
            System.out.println(
                    "ReaderListener: on_requested_deadline_missed()");
        }

        public void on_requested_incompatible_qos(
                DataReader dataReader,
                RequestedIncompatibleQosStatus status)
        {
            System.out.println(
                    "ReaderListener: on_requested_incompatible_qos()");
        }

        public void on_sample_rejected(
                DataReader dataReader,
                SampleRejectedStatus status)
        {
            System.out.println("ReaderListener: on_sample_rejected()");
        }

        public void on_liveliness_changed(
                DataReader dataReader,
                LivelinessChangedStatus status)
        {
            System.out.println("ReaderListener: on_liveliness_changed()");
            System.out.print("  Alive writers: " + status.alive_count + "\n");
        }

        public void on_sample_lost(
                DataReader dataReader,
                SampleLostStatus status)
        {
            System.out.println("ReaderListener: on_sample_lost()");
        }

        public void on_subscription_matched(
                DataReader dataReader,
                SubscriptionMatchedStatus status)
        {
            System.out.println("ReaderListener: on_subscription_matched()");
        }

        public void on_data_available(DataReader dataReader)
        {
            System.out.println("ReaderListener: on_data_available()");

            listenersDataReader listenersReader =
                    (listenersDataReader) dataReader;

            listenersSeq _dataSeq = new listenersSeq();
            SampleInfoSeq _infoSeq = new SampleInfoSeq();

            try {
                listenersReader.take(
                        _dataSeq,
                        _infoSeq,
                        ResourceLimitsQosPolicy.LENGTH_UNLIMITED,
                        SampleStateKind.ANY_SAMPLE_STATE,
                        ViewStateKind.ANY_VIEW_STATE,
                        InstanceStateKind.ANY_INSTANCE_STATE);

                for (int i = 0; i < _infoSeq.size(); ++i) {
                    SampleInfo info = (SampleInfo) _infoSeq.get(i);

                    if (info.valid_data) {
                        System.out.println(
                                "   x: " + ((listeners) _dataSeq.get(i)).x);
                    } else {
                        System.out.print("   Got metadata\n");
                    }
                    samplesRead++;
                }
            } catch (RETCODE_NO_DATA noData) {
                // No data to process
            } finally {
                listenersReader.return_loan(_dataSeq, _infoSeq);
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
        try (listenersSubscriber subscriberApplication =
                     new listenersSubscriber()) {
            subscriberApplication.parseArguments(args);
            subscriberApplication.addShutdownHook();
            subscriberApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
