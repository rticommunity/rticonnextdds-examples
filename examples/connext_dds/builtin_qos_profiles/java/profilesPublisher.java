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
import com.rti.dds.infrastructure.InstanceHandle_t;
import com.rti.dds.infrastructure.StatusKind;
import com.rti.dds.publication.Publisher;
import com.rti.dds.topic.Topic;

public class profilesPublisher extends Application implements AutoCloseable {
    // Usually one per application
    private DomainParticipant participant = null;

    private void runApplication()
    {
        // Start communicating in a domain
        participant = Objects.requireNonNull(
                DomainParticipantFactory.TheParticipantFactory
                        .create_participant(
                                getDomainId(),
                                DomainParticipantFactory
                                        .PARTICIPANT_QOS_DEFAULT,
                                null /* listener */,
                                StatusKind.STATUS_MASK_NONE));

        /* If you want to change the DomainParticipant's QoS
         * programmatically rather than using the XML file, you will need
         * to add the following lines to your code and comment out the
         * create_participant call above.
         *
         * This example uses a built-in QoS profile to enable
         * monitoring on the DomainParticipant.*/
        /* participant = DomainParticipantFactory.TheParticipantFactory.
            create_participant_with_profile(
                getDomainId(), "BuiltinQosLib", "Generic.Monitoring.Common",
                null /* listener * /, StatusKind.STATUS_MASK_NONE);
            */

        // --- Create publisher --- //

        /* To customize publisher QoS, use
           the configuration file USER_QOS_PROFILES.xml */

        // A Publisher allows an application to create one or more DataWriters
        Publisher publisher = participant.create_publisher(
                DomainParticipant.PUBLISHER_QOS_DEFAULT,
                null /* listener */,
                StatusKind.STATUS_MASK_NONE);

        // Register the datatype to use when creating the Topic
        String typeName = profilesTypeSupport.get_type_name();
        profilesTypeSupport.register_type(participant, typeName);

        // Create a Topic with a name and a datatype
        Topic topic = Objects.requireNonNull(participant.create_topic(
                "Example profiles",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));

        // This DataWriter writes data on "Example profiles" Topic
        profilesDataWriter writer = (profilesDataWriter) Objects.requireNonNull(
                publisher.create_datawriter(
                        topic,
                        Publisher.DATAWRITER_QOS_DEFAULT,
                        null /* listener */,
                        StatusKind.STATUS_MASK_NONE));

        /* If you want to change the DataWriter's QoS programatically rather
         * than using the XML file, you will need to add the following lines to
         * your code and comment out the create_datawriter call above.
         *
         * This example uses a built-in QoS profile to tune the QoS for
         * reliable streaming data. */
        /* profilesDataWriter writer = (profilesDataWriter)
           Objects.requireNonNull( publisher.create_datawriter_with_profile(
                topic,
                BuiltinQosProfiles.BUILTIN_QOS_LIB_EXP,
                BuiltinQosProfiles.PROFILE_PATTERN_RELIABLE_STREAMING,
                null /* listener * /,
                StatusKind.STATUS_MASK_NONE));
        */

        profiles data = new profiles();

        InstanceHandle_t instance_handle = InstanceHandle_t.HANDLE_NIL;
        /* For a data type that has a key, if the same instance is going to be
            written multiple times, initialize the key here
            and register the keyed instance prior to writing */
        // instance_handle = writer.register_instance(data);

        final long sendPeriodMillis = 4 * 1000;  // 4 seconds

        for (int samplesWritten = 0;
             !isShutdownRequested() && samplesWritten < getMaxSampleCount();
             samplesWritten++) {
            System.out.println("Writing profiles, count " + samplesWritten);

            // Modify the data to be written here
            data.msg = "Hello World!";

            writer.write(data, instance_handle);
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
