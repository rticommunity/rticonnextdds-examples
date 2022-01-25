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
import com.rti.dds.infrastructure.InstanceHandle_t;
import com.rti.dds.infrastructure.StatusKind;
import com.rti.dds.publication.Publisher;
import com.rti.dds.topic.Topic;

public class profilesPublisher extends Application implements AutoCloseable {
    // Usually one per application
    private DomainParticipant participant = null;

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

        /* We haven't changed the publisher_qos in any of QoS profiles we
         * use in this example, so we can just use the create_publisher()
         * method. If you want to load an specific profile in which you may
         * have changed the publisher_qos, use the
         * create_publisher_with_profile() method.
         */
        Publisher publisher =
                Objects.requireNonNull(participant.create_publisher(
                        DomainParticipant.PUBLISHER_QOS_DEFAULT,
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

        /* Volatile writer -- As volatile_profile is the default qos
         * profile we don't need to specify the profile we are going to
         * use, we can just call create_datawriter passing
         * DDS_DATAWRITER_QOS_DEFAULT.
         */
        profilesDataWriter volatileWriter =
                (profilesDataWriter) Objects.requireNonNull(
                        publisher.create_datawriter(
                                topic,
                                Publisher.DATAWRITER_QOS_DEFAULT,
                                null,  // listener
                                StatusKind.STATUS_MASK_NONE));

        /* Transient Local writer -- In this case we use
         * create_datawriter_with_profile, because we have to use a profile
         * other than the default one. This profile has been defined in
         * my_custom_qos_profiles.xml, but since we already loaded the XML
         * file we don't need to specify anything else.
         */
        profilesDataWriter transientLocalWriter =
                (profilesDataWriter) Objects.requireNonNull(
                        publisher.create_datawriter_with_profile(
                                topic,
                                "profiles_Library",
                                "transient_local_profile",
                                null /* listener */,
                                StatusKind.STATUS_MASK_NONE));

        // Create data sample for writing
        profiles data = new profiles();

        InstanceHandle_t instance_handle = InstanceHandle_t.HANDLE_NIL;
        /* For a data type that has a key, if the same instance is going to be
         * written multiple times, initialize the key here
         * and register the keyed instance prior to writing
         */
        // instance_handle = writer.register_instance(data);

        final long sendPeriodMillis = 4 * 1000;  // 4 seconds

        for (int samplesWritten = 0;
             !isShutdownRequested() && samplesWritten < getMaxSampleCount();
             samplesWritten++) {
            System.out.println("Writing profiles, count " + samplesWritten);

            // Modify the instance to be written here
            data.profile_name = "volatile_profile";
            data.x = samplesWritten;

            System.out.println(
                    "Writing profile_name = " + data.profile_name
                    + " x = " + data.x);

            // Write data
            volatileWriter.write(data, instance_handle);

            data.profile_name = "transient_local_profile";

            System.out.println(
                    "Writing profile_name = " + data.profile_name
                    + " x = " + data.x);

            // Write data
            transientLocalWriter.write(data, instance_handle);

            try {
                Thread.sleep(sendPeriodMillis);
            } catch (InterruptedException ix) {
                System.err.println("INTERRUPTED");
                break;
            }
        }
        // writer.unregister_instance(data, instance_handle);
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
        try (profilesPublisher publisherApplication = new profilesPublisher()) {
            publisherApplication.parseArguments(args);
            publisherApplication.addShutdownHook();
            publisherApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
