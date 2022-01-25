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
import com.rti.dds.publication.DataWriter;
import com.rti.dds.publication.DataWriterAdapter;
import com.rti.dds.publication.LivelinessLostStatus;
import com.rti.dds.publication.OfferedDeadlineMissedStatus;
import com.rti.dds.publication.OfferedIncompatibleQosStatus;
import com.rti.dds.publication.PublicationMatchedStatus;
import com.rti.dds.publication.Publisher;
import com.rti.dds.publication.ReliableReaderActivityChangedStatus;
import com.rti.dds.publication.ReliableWriterCacheChangedStatus;
import com.rti.dds.topic.Topic;


public class listenersPublisher extends Application implements AutoCloseable {
    // Usually one per application
    private DomainParticipant participant = null;

    private void runApplication()
    {
        // Start communicating in a domain
        participant = Objects.requireNonNull(
                DomainParticipantFactory.get_instance().create_participant(
                        getDomainId(),
                        DomainParticipantFactory.PARTICIPANT_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        // A Publisher allows an application to create one or more DataWriters
        Publisher publisher =
                Objects.requireNonNull(participant.create_publisher(
                        DomainParticipant.PUBLISHER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        /* Create ande Delete Inconsistent Topic
         * ---------------------------------------------------------------
         * Here we create an inconsistent topic to trigger the subscriber
         * application's callback.
         * The inconsistent topic is created with the topic name used in
         * the Subscriber application, but with a different data type --
         * the msg data type defined in partitions.idl.
         * Once it is created, we sleep to ensure the applications discover
         * each other and delete the Data Writer and Topic.
         */

        /* First we register the msg type -- we name it
         * inconsistent_topic_type_name to avoid confusion.
         */
        System.out.println("Creating Inconsistent Topic...  ");

        // Register a different type than subscriber is expecting
        String inconsistentTypeName = msgTypeSupport.get_type_name();
        msgTypeSupport.register_type(participant, inconsistentTypeName);

        Topic inconsistentTopic =
                Objects.requireNonNull(participant.create_topic(
                        "Example listeners",
                        inconsistentTypeName,
                        DomainParticipant.TOPIC_QOS_DEFAULT,
                        null /* listener */,
                        StatusKind.STATUS_MASK_NONE));

        /* We have to associate a writer to the topic, as Topic information is
         * not actually propagated until the creation of an associated writer.
         */
        msgDataWriter inconsistentWriter =
                (msgDataWriter) Objects.requireNonNull(
                        publisher.create_datawriter(
                                inconsistentTopic,
                                Publisher.DATAWRITER_QOS_DEFAULT,
                                null /* listener */,
                                StatusKind.STATUS_MASK_NONE));


        // Sleep to ensure the apps have had time to discover each other
        long sleepPeriodMillis = 2000;
        try {
            Thread.sleep(sleepPeriodMillis);
        } catch (InterruptedException ix) {
            System.err.println("INTERRUPTED");
        }


        publisher.delete_datawriter(inconsistentWriter);
        participant.delete_topic(inconsistentTopic);

        System.out.println("... Deleted Inconsistent Topic\n");

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

        /* We will use the Data Writer Listener defined above to print
         * a message when some of events are triggered in the DataWriter.
         * To do that, first we have to pass the writer_listener and then
         * we have to enable all status in the status mask.
         */
        DataWriterListener writerListener = new DataWriterListener();
        listenersDataWriter writer = (listenersDataWriter) Objects.requireNonNull(
                publisher.create_datawriter(
                        topic,
                        Publisher.DATAWRITER_QOS_DEFAULT,
                        writerListener /* listener */,
                        StatusKind.STATUS_MASK_ALL /* get all statuses */));

        // Create data sample for writing
        listeners data = new listeners();

        InstanceHandle_t instance_handle = InstanceHandle_t.HANDLE_NIL;
        /* For a data type that has a key, if the same instance is going to be
         * written multiple times, initialize the key here
         * and register the keyed instance prior to writing
         */
        // instance_handle = writer.register_instance(data);

        final long sendPeriodMillis = 1000;  // 1 second

        for (int samplesWritten = 0;
             !isShutdownRequested() && samplesWritten < getMaxSampleCount();
             samplesWritten++) {
            System.out.println("Writing listeners, count " + samplesWritten);

            // Modify the instance to be written here
            data.x = (short) samplesWritten;

            // Write data
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

    private static class DataWriterListener extends DataWriterAdapter {
        public void on_offered_deadline_missed(
                DataWriter dataWriter,
                OfferedDeadlineMissedStatus status)
        {
            System.out.println(
                    "DataWriterListener: on_offered_deadline_missed()\n");
        }
        public void on_liveliness_lost(
                DataWriter writer,
                LivelinessLostStatus status)
        {
            System.out.println("DataWriterListener: on_liveliness_lost()\n");
        }

        public void on_offered_incompatible_qos(
                DataWriter writer,
                OfferedIncompatibleQosStatus status)
        {
            System.out.println(
                    "DataWriterListener: on_offered_incompatible_qos()\n");
        }

        public void on_publication_matched(
                DataWriter writer,
                PublicationMatchedStatus status)
        {
            System.out.println(
                    "DataWriterListener: on_publication_matched()\n");
            if (status.current_count_change < 0) {
                System.out.println("lost a subscription\n");
            } else {
                System.out.println("found a subscription\n");
            }
        }

        public void on_reliable_writer_cache_changed(
                DataWriter writer,
                ReliableWriterCacheChangedStatus status)
        {
            System.out.println(
                    "DataWriterListener: on_reliable_writer_cache_changed()\n");
        }

        public void on_reliable_reader_activity_changed(
                DataWriter writer,
                ReliableReaderActivityChangedStatus status)
        {
            System.out.println(
                    "DataWriterListener: on_reliable_reader_activity_changed()\n");
        }
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
        try (listenersPublisher publisherApplication =
                     new listenersPublisher()) {
            publisherApplication.parseArguments(args);
            publisherApplication.addShutdownHook();
            publisherApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
