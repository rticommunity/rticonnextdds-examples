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

import java.util.Arrays;
import java.util.Objects;

import com.rti.dds.domain.DomainParticipant;
import com.rti.dds.domain.DomainParticipantFactory;
import com.rti.dds.domain.builtin.ParticipantBuiltinTopicData;
import com.rti.dds.domain.builtin.ParticipantBuiltinTopicDataSeq;
import com.rti.dds.domain.builtin.ParticipantBuiltinTopicDataDataReader;
import com.rti.dds.domain.builtin.ParticipantBuiltinTopicDataTypeSupport;
import com.rti.dds.infrastructure.InstanceHandle_t;
import com.rti.dds.infrastructure.StatusKind;
import com.rti.dds.infrastructure.RETCODE_NO_DATA;
import com.rti.dds.infrastructure.ResourceLimitsQosPolicy;
import com.rti.dds.publication.Publisher;
import com.rti.dds.subscription.Subscriber;
import com.rti.dds.subscription.DataReader;
import com.rti.dds.subscription.DataReaderAdapter;
import com.rti.dds.subscription.InstanceStateKind;
import com.rti.dds.subscription.SampleInfo;
import com.rti.dds.subscription.SampleInfoSeq;
import com.rti.dds.subscription.SampleStateKind;
import com.rti.dds.subscription.ViewStateKind;
import com.rti.dds.subscription.builtin.SubscriptionBuiltinTopicData;
import com.rti.dds.subscription.builtin.SubscriptionBuiltinTopicDataSeq;
import com.rti.dds.subscription.builtin.SubscriptionBuiltinTopicDataDataReader;
import com.rti.dds.subscription.builtin.SubscriptionBuiltinTopicDataTypeSupport;
import com.rti.dds.topic.Topic;


// ===========================================================================

public class msgPublisher extends Application implements AutoCloseable {

    public static String auth = "password";
    private DomainParticipant participant = null;

    /* The builtin subscriber sets participant_qos.user_data and
       reader_qos.user_data, so we set up listeners for the builtin
       DataReaders to access these fields.
    */
    public static class BuiltinParticipantListener extends DataReaderAdapter {
        ParticipantBuiltinTopicDataSeq _dataSeq =
                new ParticipantBuiltinTopicDataSeq();
        SampleInfoSeq _infoSeq = new SampleInfoSeq();

        // This gets called when a participant has been discovered
        public void on_data_available(DataReader reader) {
            ParticipantBuiltinTopicDataDataReader builtin_reader =
                (ParticipantBuiltinTopicDataDataReader)reader;
            String participant_data;
            ParticipantBuiltinTopicData cur_participant_builtin_topic_data;

            try {

                // We only process newly seen participants
                builtin_reader.take(
                    _dataSeq, _infoSeq,
                    ResourceLimitsQosPolicy.LENGTH_UNLIMITED,
                    SampleStateKind.ANY_SAMPLE_STATE,
                    ViewStateKind.NEW_VIEW_STATE,
                    InstanceStateKind.ANY_INSTANCE_STATE);

                for(int i = 0; i < _dataSeq.size(); ++i) {
                    SampleInfo info = (SampleInfo)_infoSeq.get(i);

                    if (info.valid_data) {
                        participant_data = "nil";
                        boolean is_auth = false;
                        cur_participant_builtin_topic_data =
                                (ParticipantBuiltinTopicData)_dataSeq.get(i);

                        // see if there is any participant_data
                        if (cur_participant_builtin_topic_data.
                                user_data.value.size() > 0) {

                            // This sequence is guaranteed to be contiguous
                            participant_data =
                                    new String(
                                            cur_participant_builtin_topic_data.
                                            user_data.value.toArrayByte(null));
                            is_auth = participant_data.equals(auth);
                        }

                        System.out.println("Builtin Reader: found participant");
                        System.out.println("\tkey->'" + Arrays.toString(
                                cur_participant_builtin_topic_data.key.value) +
                                "'\n\tuser_data->'" + participant_data + "'");
                        System.out.println("instance_handle: " +
                                info.instance_handle);

                        if (!is_auth) {
                            System.out.println(
                                    "Bad authorization, ignoring participant");
                            DomainParticipant participant =
                                    reader.get_subscriber().get_participant();
                            participant.ignore_participant(
                                    info.instance_handle);
                        }
                    }
                }

            } catch (RETCODE_NO_DATA noData) {
                // No data to process
                // This happens when we get announcements from participants we
                // already know about
                return;
            } finally {
                builtin_reader.return_loan(_dataSeq, _infoSeq);
            }
        }
    }

    public static class BuiltinSubscriberListener extends DataReaderAdapter {
        SubscriptionBuiltinTopicDataSeq _dataSeq =
                new SubscriptionBuiltinTopicDataSeq();
        SampleInfoSeq _infoSeq = new SampleInfoSeq();

        // This gets called when a new subscriber has been discovered
        public void on_data_available(DataReader reader) {
            SubscriptionBuiltinTopicDataDataReader builtin_reader =
                (SubscriptionBuiltinTopicDataDataReader)reader;
            SubscriptionBuiltinTopicData cur_subscription_builtin_topic_data;

            try {

                // We only process newly seen subscribers
                builtin_reader.take(
                    _dataSeq, _infoSeq,
                    ResourceLimitsQosPolicy.LENGTH_UNLIMITED,
                    SampleStateKind.ANY_SAMPLE_STATE,
                    ViewStateKind.NEW_VIEW_STATE,
                    InstanceStateKind.ANY_INSTANCE_STATE);

                for(int i = 0; i < _dataSeq.size(); ++i) {
                    SampleInfo info = (SampleInfo)_infoSeq.get(i);

                    if (info.valid_data) {
                        cur_subscription_builtin_topic_data =
                                (SubscriptionBuiltinTopicData)_dataSeq.get(i);


                        System.out.println("Built-in Reader: found subscriber");
                        System.out.println("\tparticipant_key->'"
                                + Arrays.toString(
                                        cur_subscription_builtin_topic_data.
                                        participant_key.value) + "'\n\tkey->'"
                                + Arrays.toString(
                                        cur_subscription_builtin_topic_data.
                                        key.value)
                                + "\n");

                        System.out.println("instance_handle: " +
                                info.instance_handle);
                    }
                }

            } catch (RETCODE_NO_DATA noData) {
                // No data to process
                return;
            } finally {
                builtin_reader.return_loan(_dataSeq, _infoSeq);
            }
        }
    }

    private void runApplication() {

        //// Start changes for Builtin_Topics
        /* If you want to change the Factory's QoS programmatically rather
         * than using the XML file, you will need to add the following lines
         * to your code and comment out the participant call above.
         */

        /* By default, the participant is enabled upon construction.
         * At that time our listeners for the builtin topics have not
         * been installed, so we disable the participant until we
         * set up the listeners.
         */
        /*
        DomainParticipantFactoryQos factoryQos =
                new DomainParticipantFactoryQos();
        DomainParticipantFactory.TheParticipantFactory.get_qos(factoryQos);
        factoryQos.entity_factory.autoenable_created_entities = false;
        DomainParticipantFactory.TheParticipantFactory.set_qos(factoryQos);
        */
        // --- Create participant --- //

        // If you want to change the Participant's QoS programmatically
        // rather than using the XML file, you will need to uncomment the
        // following lines and replace in create_participant the argument
        // DomainParticipantFactory.PARTICIPANT_QOS_DEFAULT with
        // participantQos
        //DomainParticipantQos participantQos = new DomainParticipantQos();
        //DomainParticipantFactory.TheParticipantFactory
        //        .get_default_participant_qos(participantQos);
        //participantQos.resource_limits.participant_user_data_max_length = 1024;

        // Start communicating in a domain
        participant = Objects.requireNonNull(
                DomainParticipantFactory.get_instance().
                create_participant(
                    getDomainId(), DomainParticipantFactory.PARTICIPANT_QOS_DEFAULT,
                    null /* listener */, StatusKind.STATUS_MASK_NONE));

        /* Installing listeners for the builtin topics requires several
         * steps
         */

        // First get the builtin subscriber
        Subscriber builtin_subscriber =
            Objects.requireNonNull(participant.get_builtin_subscriber());

        /*  Then get builtin subscriber's datareader for participants
            The type name is a bit hairy, but can be read right to left:
            DDSParticipantBuiltinTopicDataDataReader is a
            DataReader for BuiltinTopicData concerning a discovered
            Participant
        */
        ParticipantBuiltinTopicDataDataReader
            builtin_participant_datareader =
                    (ParticipantBuiltinTopicDataDataReader) Objects.requireNonNull(
                        builtin_subscriber.lookup_datareader(
                                ParticipantBuiltinTopicDataTypeSupport.
                                PARTICIPANT_TOPIC_NAME));

        // Install our listener
        BuiltinParticipantListener builtin_participant_listener =
                new BuiltinParticipantListener();
        builtin_participant_datareader.set_listener(
                builtin_participant_listener,
                StatusKind.DATA_AVAILABLE_STATUS);

        // Get builtin subscriber's datareader for subscribers
        SubscriptionBuiltinTopicDataDataReader
                builtin_subscription_datareader =
                    (SubscriptionBuiltinTopicDataDataReader) Objects.requireNonNull(
                        builtin_subscriber.lookup_datareader(
                                SubscriptionBuiltinTopicDataTypeSupport.
                                SUBSCRIPTION_TOPIC_NAME));

        // Install our listener
        BuiltinSubscriberListener builtin_subscriber_listener =
                new BuiltinSubscriberListener();
        builtin_subscription_datareader.set_listener(
                builtin_subscriber_listener,
                StatusKind.DATA_AVAILABLE_STATUS);

        /* Done!  All the listeners are installed, so we can enable the
         * participant now.
         */
        participant.enable();

        Publisher publisher = Objects.requireNonNull(participant.create_publisher(
                DomainParticipant.PUBLISHER_QOS_DEFAULT, null /* listener */,
                StatusKind.STATUS_MASK_NONE));


        // Register the datatype to use when creating the Topic
        String typeName = msgTypeSupport.get_type_name();
        msgTypeSupport.register_type(participant, typeName);

        // Create a Topic with a name and a datatype
        Topic topic = Objects.requireNonNull(participant.create_topic(
            "Example msg",
            typeName, DomainParticipant.TOPIC_QOS_DEFAULT,
            null /* listener */, StatusKind.STATUS_MASK_NONE));

        // This DataWriter writes data on "Example msg" Topic
        msgDataWriter writer = (msgDataWriter) Objects.requireNonNull(
                publisher.create_datawriter(
                    topic, Publisher.DATAWRITER_QOS_DEFAULT,
                    null, StatusKind.STATUS_MASK_NONE));

        // Create data sample for writing
        msg data = new msg();

        InstanceHandle_t instance_handle = InstanceHandle_t.HANDLE_NIL;
        /* For a data type that has a key, if the same instance is going
         * to be written multiple times, initialize the key here
         * and register the keyed instance prior to writing
         */
        //instance_handle = writer.register_instance(data);

        //// Changes for Builtin_Topics
        final long sendPeriodMillis = 1000; // 1 second

        for (short samplesWritten = 0;
             !isShutdownRequested() && samplesWritten < getMaxSampleCount();
             samplesWritten++) {
            System.out.println("Writing msg, count " + samplesWritten);

            // Modify the data to be written here
            data.x = samplesWritten;

            writer.write(data, instance_handle);
            try {
                Thread.sleep(sendPeriodMillis);
            } catch (InterruptedException ix) {
                System.err.println("INTERRUPTED");
                break;
            }
        }

        //writer.unregister_instance(instance, instance_handle);
    }

    @Override public void close()
    {
        // Delete all entities (DataWriter, Topic, Publisher, DomainParticipant)
        if (participant != null) {
            participant.delete_contained_entities();

            DomainParticipantFactory.get_instance().delete_participant(
                    participant);
        }
    }

    public static void main(String[] args)
    {
        // Create example and run: Uses try-with-resources,
        // publisherApplication.close() automatically called
        try (msgPublisher publisherApplication = new msgPublisher()) {
            publisherApplication.parseArguments(args, ApplicationType.PUBLISHER);
            publisherApplication.addShutdownHook();
            publisherApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
