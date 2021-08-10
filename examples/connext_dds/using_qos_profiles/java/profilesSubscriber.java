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
import com.rti.dds.domain.DomainParticipantFactoryQos;
import com.rti.dds.infrastructure.ConditionSeq;
import com.rti.dds.infrastructure.Duration_t;
import com.rti.dds.infrastructure.RETCODE_NO_DATA;
import com.rti.dds.infrastructure.RETCODE_TIMEOUT;
import com.rti.dds.infrastructure.ResourceLimitsQosPolicy;
import com.rti.dds.infrastructure.StatusKind;
import com.rti.dds.infrastructure.WaitSet;
import com.rti.dds.subscription.DataReader;
import com.rti.dds.subscription.DataReaderAdapter;
import com.rti.dds.subscription.DataReaderListener;
import com.rti.dds.subscription.InstanceStateKind;
import com.rti.dds.subscription.ReadCondition;
import com.rti.dds.subscription.SampleInfo;
import com.rti.dds.subscription.SampleInfoSeq;
import com.rti.dds.subscription.SampleStateKind;
import com.rti.dds.subscription.Subscriber;
import com.rti.dds.subscription.ViewStateKind;
import com.rti.dds.topic.Topic;

public class profilesSubscriber extends Application implements AutoCloseable {
    // Usually one per application
    private DomainParticipant participant = null;
    private static int samplesRead = 0;

    private void runApplication()
    {
        /* There are several different approaches for loading QoS profiles
         * from XML files (see Configuring QoS with XML chapter in the RTI
         * Connext Core Libraries and Utilities User's Manual). In this
         * example we illustrate two of them:
         *
         * 1) Creating a file named USER_QOS_PROFILES.xml, which is loaded,
         * automatically by the DomainParticipantFactory. In this case, the
         * file defines a QoS profile named volatile_profile that
         * configures reliable, volatile DataWriters and DataReaders.
         *
         * 2) Adding XML documents to the DomainParticipantFactory using
         * its Profile QoSPolicy (DDS Extension). In this case, we add
         * my_custom_qos_profiles.xml to the url_profile sequence, which
         * stores the URLs of all the XML documents with QoS policies that
         * are loaded by the DomainParticipantFactory aside from the ones
         * that are automatically loaded.
         * my_custom_qos_profiles.xml defines a QoS profile named
         * transient_local_profile that configures reliable, transient
         * local DataWriters and DataReaders.
         */
        /* To load my_custom_qos_profiles.xml, as explained above, we need
         * to modify the DDSTheParticipantFactory Profile QoSPolicy
         */
        DomainParticipantFactoryQos factoryQos =
                new DomainParticipantFactoryQos();
        DomainParticipantFactory.TheParticipantFactory.get_qos(factoryQos);

        /* We are only going to add one XML file to the url_profile
         * sequence, so we set a maximum length of 1.
         */
        factoryQos.profile.url_profile.setMaximum(1);

        /* The XML file will be loaded from the working directory. That
         * means, you need to run the example like this:
         * ./objs/<architecture>/profiles_publisher
         * (see README.txt for more information on how to run the example).
         *
         * Note that you can specify the absolute path of the XML QoS file
         * to avoid this problem.
         */
        factoryQos.profile.url_profile.add("file://my_custom_qos_profiles.xml");
        DomainParticipantFactory.TheParticipantFactory.set_qos(factoryQos);

        /* Our default Qos profile, volatile_profile, sets the participant
         * name. This is the only participant_qos policy that we change in
         * our example. As this is done in the default QoS profile, we
         * don't need to specify its name, so we can create the participant
         * using the create_participant() method rather than using
         * create_participant_with_profile().
         */
        participant = Objects.requireNonNull(
                DomainParticipantFactory.get_instance().create_participant(
                        getDomainId(),
                        DomainParticipantFactory.PARTICIPANT_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        /* We haven't changed the subscriber_qos in any of QoS profiles we
         * use in this example, so we can just use the create_subscriber()
         * method. If you want to load an specific profile in which you may
         * have changed the subscriber_qos, use the
         * create_subscriber_with_profile() method.
         */
        Subscriber subscriber =
                Objects.requireNonNull(participant.create_subscriber(
                        DomainParticipant.SUBSCRIBER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        // Register type before creating topic
        String typeName = profilesTypeSupport.get_type_name();
        profilesTypeSupport.register_type(participant, typeName);

        /* We haven't changed the topic_qos in any of QoS profiles we use
         * in this example, so we can just use the create_topic() method.
         * If you want to load an specific profile in which you may have
         * changed the topic_qos, use the create_topic_with_profile()
         * method.
         */
        Topic topic = Objects.requireNonNull(participant.create_topic(
                "Example profiles",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));

        DataReaderListener volatileReaderListener =
                new profilesListener("VolatileProfile");
        DataReaderListener transientLocalReaderListener =
                new profilesListener("TransientLocalProfile");

        /* Volatile reader -- As volatile_profile is the default QoS
         * profile we don't need to specify the profile we are going to
         * use, we can just call create_datareader passing
         * DDS_DATAWRITER_QOS_DEFAULT.
         */
        profilesDataReader volatileReader =
                (profilesDataReader) Objects.requireNonNull(
                        subscriber.create_datareader(
                                topic,
                                Subscriber.DATAREADER_QOS_DEFAULT,
                                volatileReaderListener,
                                StatusKind.STATUS_MASK_ALL));

        /* Transient Local reader -- In this case we use
         * create_datareader_with_profile, because we have to use a profile
         * other than the default one. This profile has been defined in
         * my_custom_qos_profiles.xml, but since we already loaded the XML
         * file we don't need to specify anything else.
         */
        profilesDataReader transientLocalReader =
                (profilesDataReader) Objects.requireNonNull(
                        subscriber.create_datareader_with_profile(
                                topic,
                                "profiles_Library",
                                "transient_local_profile",
                                transientLocalReaderListener,
                                StatusKind.STATUS_MASK_ALL));

        final long receivePeriodSec = 4;

        // Main loop. Wait for data to arrive and process when it arrives
        while (!isShutdownRequested() && samplesRead < getMaxSampleCount()) {
            System.out.println(
                    "profiles subscriber sleeping for " + receivePeriodSec
                    + " sec...");
            try {
                Thread.sleep(receivePeriodSec * 1000);  // in millisec
            } catch (InterruptedException ix) {
                System.err.println("INTERRUPTED");
                break;
            }
        }
    }

    private static class profilesListener extends DataReaderAdapter {
        profilesSeq dataSeq = new profilesSeq();
        SampleInfoSeq infoSeq = new SampleInfoSeq();
        String listenerName = new String();

        public profilesListener(String profileName)
        {
            listenerName = profileName;
        }

        public void on_data_available(DataReader reader)
        {
            profilesDataReader profilesReader = (profilesDataReader) reader;

            try {
                profilesReader.take(
                        dataSeq,
                        infoSeq,
                        ResourceLimitsQosPolicy.LENGTH_UNLIMITED,
                        SampleStateKind.ANY_SAMPLE_STATE,
                        ViewStateKind.ANY_VIEW_STATE,
                        InstanceStateKind.ANY_INSTANCE_STATE);

                System.out.println("========================================");
                System.out.println(listenerName + " listener received\n");
                System.out.println("======================================\n");
                for (int i = 0; i < dataSeq.size(); ++i) {
                    SampleInfo info = (SampleInfo) infoSeq.get(i);

                    if (info.valid_data) {
                        System.out.println(((profiles) dataSeq.get(i))
                                                   .toString("Received", 0));
                    }
                    samplesRead++;
                }
            } catch (RETCODE_NO_DATA noData) {
                // No data to process
            } finally {
                profilesReader.return_loan(dataSeq, infoSeq);
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
        try (profilesSubscriber subscriberApplication =
                     new profilesSubscriber()) {
            subscriberApplication.parseArguments(args);
            subscriberApplication.addShutdownHook();
            subscriberApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
