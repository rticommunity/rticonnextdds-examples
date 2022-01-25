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
import com.rti.dds.subscription.InstanceStateKind;
import com.rti.dds.subscription.ReadCondition;
import com.rti.dds.subscription.SampleInfo;
import com.rti.dds.subscription.SampleInfoSeq;
import com.rti.dds.subscription.SampleStateKind;
import com.rti.dds.subscription.Subscriber;
import com.rti.dds.subscription.ViewStateKind;
import com.rti.dds.topic.Topic;


public class orderedSubscriber extends Application implements AutoCloseable {
    private static int MAX_SUBSCRIBERS = 2;
    private DomainParticipant participant = null;  // Usually one per
                                                   // application
    private orderedDataReader reader = null;
    private final orderedSeq dataSeq = new orderedSeq();
    private final SampleInfoSeq infoSeq = new SampleInfoSeq();

    // No listener is needed; we poll readers in this function
    private int poll_data(DataReader[] reader, int numreaders)
    {
        orderedSeq dataSeq = new orderedSeq();
        SampleInfoSeq infoSeq = new SampleInfoSeq();
        int samplesRead = 0;

        for (int r = 0; r < numreaders; ++r) {
            try {
                ((orderedDataReader) (reader[r]))
                        .take(dataSeq,
                              infoSeq,
                              ResourceLimitsQosPolicy.LENGTH_UNLIMITED,
                              SampleStateKind.ANY_SAMPLE_STATE,
                              ViewStateKind.ANY_VIEW_STATE,
                              InstanceStateKind.ANY_INSTANCE_STATE);

                for (int i = 0; i < dataSeq.size(); ++i) {
                    SampleInfo info = (SampleInfo) infoSeq.get(i);

                    if (!info.valid_data) {
                        continue;
                    } else {
                        // Make things a bit easier to read.
                        int ident = r;
                        while (ident-- != 0) {
                            System.out.print("\t");
                        }
                        System.out.println(
                                "Reader " + r + ": Instance"
                                + ((ordered) dataSeq.get(i)).id + "->value = "
                                + ((ordered) dataSeq.get(i)).value);
                    }
                    samplesRead++;
                }
            } catch (RETCODE_NO_DATA noData) {
                // Not an error
                return samplesRead;
            } catch (RETCODE_ERROR e) {
                // Is an error
                System.out.println("take error " + e);
                return samplesRead;
            } finally {
                ((orderedDataReader) (reader[r])).return_loan(dataSeq, infoSeq);
                infoSeq.setMaximum(0);
                dataSeq.setMaximum(0);
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

        // Register type before creating topic
        String typeName = orderedTypeSupport.get_type_name();
        orderedTypeSupport.register_type(participant, typeName);

        // Create a Topic with a name and a datatype
        Topic topic = Objects.requireNonNull(participant.create_topic(
                "Example ordered",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));

        // Start changes for Ordered_Presentation

        /*
         * Create two subscribers to illustrate different presentation QoS
         * This is a publisher/subscriber level QoS, so we have to do it
         * here instead of just making two datareaders
         */
        Subscriber[] subscriber = new Subscriber[MAX_SUBSCRIBERS];
        DataReader[] reader = new DataReader[MAX_SUBSCRIBERS];
        String[] profile_name = { "ordered_Profile_subscriber_instance",
                                  "ordered_Profile_subscriber_topic" };

        /*
         * Subscriber[0], reader[0] and ordered_reader[0] is getting the
         * profile "ordered_Profile_subscriber_instance"
         */
        /*
         * Subscriber[1], reader[1] and ordered_reader[1] is getting the
         * profile "ordered_Profile_subscriber_topic"
         */

        for (int i = 0; i < MAX_SUBSCRIBERS; ++i) {
            System.out.println("Subscriber " + i + " using " + profile_name[i]);

            subscriber[i] = Objects.requireNonNull(
                    participant.create_subscriber_with_profile(
                            "ordered_Library",
                            profile_name[i],
                            null,
                            StatusKind.STATUS_MASK_NONE));

            reader[i] = Objects.requireNonNull(
                    subscriber[i].create_datareader_with_profile(
                            topic,
                            "ordered_Library",
                            profile_name[i],
                            null,
                            StatusKind.STATUS_MASK_ALL));
        }

        /*
         * If you want to change the Publisher's QoS programmatically rather
         * than using the XML file, you will need to add the following lines
         * to your code and comment out the above 'for' loop.
         */
        // Get default subscriber QoS to customize
        /*
        SubscriberQos subscriber_qos = new SubscriberQos();
        participant.get_default_subscriber_qos(subscriber_qos);

        // Set this for both subscribers
        subscriber_qos.presentation.ordered_access = true;

        // No listener needed, but we do need to increase history depth
        */
        // Get default datareader QoS to customize
        /*
        DataReaderQos datareader_qos = new DataReaderQos();

        for (int i = 0; i < MAX_SUBSCRIBERS; ++i) {
            if (i == 0) {
                System.out.println(
                        "Subscriber 0 using Instance access scope");
                subscriber_qos.presentation.access_scope =
                        PresentationQosPolicyAccessScopeKind
                            .INSTANCE_PRESENTATION_QOS;
            } else {
                System.out.println("Subscriber 1 using Topic access scope");
                subscriber_qos.presentation.access_scope =
                        PresentationQosPolicyAccessScopeKind
                            .TOPIC_PRESENTATION_QOS;
            }
        }
            /*
             * To create subscriber with default QoS, use
             * DDS.DomainParticipant.SUBSCRIBER_QOS_DEFAULT instead of
             * subscriber_qos
             */
        /*
            subscriber[i] = Objects.requireNonNull(
                    participant.create_subscriber(
                            subscriber_qos,
                            null,
                            StatusKind.STATUS_MASK_NONE));

            subscriber[i].get_default_datareader_qos(datareader_qos);
            datareader_qos.history.depth = 10;
        */
        /*
         * To create datareader with default QoS, use
         * DDS.Subscriber.DATAREADER_QOS_DEFAULT instead of
         * datareader_qos
         */
        /*
            reader[i] = (orderedDataReader) Objects.requireNonNull(
                subscriber[i].create_datareader(
                        topic,
                        datareader_qos,
                        null,
                        StatusKind.STATUS_MASK_NONE));
        }
        */

        int samplesRead = 0;
        final long receivePeriodSec = 4;

        // Main loop. Wait for data to arrive and process when it arrives
        while (!isShutdownRequested() && samplesRead < getMaxSampleCount()) {
            System.out.println(
                    "ordered subscriber sleeping for " + receivePeriodSec
                    + " sec...");
            try {
                Thread.sleep(receivePeriodSec * 1000);  // in millisec
            } catch (InterruptedException ix) {
                System.err.println("INTERRUPTED");
                break;
            }
            samplesRead += poll_data(reader, 2);
        }
        // End changes for Ordered_Presentation
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
        try (orderedSubscriber subscriberApplication =
                     new orderedSubscriber()) {
            subscriberApplication.parseArguments(args);
            subscriberApplication.addShutdownHook();
            subscriberApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
