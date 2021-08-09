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
import com.rti.dds.publication.PublisherQos;
import com.rti.dds.topic.Topic;

public class partitionsPublisher extends Application implements AutoCloseable {
    // Usually one per application
    private DomainParticipant participant = null;

    private void runApplication() {

        // Start communicating in a domain
        participant = Objects.requireNonNull(
                DomainParticipantFactory.get_instance().create_participant(
                        getDomainId(),
                        DomainParticipantFactory.PARTICIPANT_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));
	    
	    PublisherQos publisher_qos = new PublisherQos();
        participant.get_default_publisher_qos(publisher_qos);

	    /* If you want to change the Partition name programmatically rather than
	     * using the XML, you will need to add the following lines to your code
	     * and comment out the create_publisher() call bellow.
	     */
	    /*
	    publisher_qos.partition.name.clear();
        publisher_qos.partition.name.add("ABC");
        publisher_qos.partition.name.add("foo");
	    
	    publisher = participant.create_publisher(publisher_qos, 
						     null,
						     StatusKind.STATUS_MASK_NONE);
	    */

        // A Publisher allows an application to create one or more DataWriters
        Publisher publisher =
                Objects.requireNonNull(participant.create_publisher(
                        DomainParticipant.PUBLISHER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

	    System.out.print("Setting partition to '" +
                publisher_qos.partition.name.get(0) + "', '" +
                publisher_qos.partition.name.get(1) + "'...\n");

        // Register type before creating topic
        String typeName = partitionsTypeSupport.get_type_name();
        partitionsTypeSupport.register_type(participant, typeName);
    
        // Create a Topic with a name and a datatype
        Topic topic = Objects.requireNonNull(participant.create_topic(
                "Example partitions",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));

	    /* In this example we set a Reliable datawriter, with Transient Local 
	     * durability. By default we set up these QoS settings via XML. If you
	     * want to to it programmatically, use the following code, and comment out
	     * the create_datawriter call bellow.
	     */
	    /*
        DataWriterQos datawriter_qos = new DataWriterQos();
        publisher.get_default_datawriter_qos(datawriter_qos);

        datawriter_qos.reliability.kind =
            ReliabilityQosPolicyKind.RELIABLE_RELIABILITY_QOS;
        datawriter_qos.durability.kind = 
            DurabilityQosPolicyKind.TRANSIENT_LOCAL_DURABILITY_QOS;
        datawriter_qos.history.kind = 
            HistoryQosPolicyKind.KEEP_LAST_HISTORY_QOS;
        datawriter_qos.history.depth = 3;

        writer = (partitionsDataWriter) Objects.requireNonNull(
                publisher.create_datawriter(
                        topic,
                        datawriter_qos,
                        null,
                        StatusKind.STATUS_MASK_NONE));
	    */

        partitionsDataWriter writer = (partitionsDataWriter) Objects.requireNonNull(
                publisher.create_datawriter(
                        topic,
                        Publisher.DATAWRITER_QOS_DEFAULT,
                        null,
                        StatusKind.STATUS_MASK_NONE));

        // Create data sample for writing
        partitions data = new partitions();

        InstanceHandle_t instance_handle = InstanceHandle_t.HANDLE_NIL;
        /* For a data type that has a key, if the same instance is going to be
         * written multiple times, initialize the key here
         * and register the keyed instance prior to writing
         */
        //instance_handle = writer.register_instance(data);

        final long sendPeriodMillis = 1000; // 1 second

        for (int samplesWritten = 0;
             !isShutdownRequested() && samplesWritten < getMaxSampleCount();
             samplesWritten++) {
                
            System.out.println("Writing partitions, count " + samplesWritten);
		    data.x = samplesWritten;
            writer.write(data, instance_handle);
            try {
                Thread.sleep(sendPeriodMillis);
            } catch (InterruptedException ix) {
                System.err.println("INTERRUPTED");
                break;
            }

		    /* Every 5 samples we will change the Partition name. These are the
		     * partition expressions we are going to try: 
		     * "bar", "A*", "A?C", "X*Z", "zzz", "A*C"
		     */
            if ((samplesWritten+1) % 25 == 0) {
                // Matches "ABC" -- name[1] here can match name[0] there, 
                // as long as there is some overlapping name
                publisher_qos.partition.name.set(0, "zzz");
                publisher_qos.partition.name.set(1, "A*C");
                System.out.print("Setting partition to '" + 
                        publisher_qos.partition.name.get(0) + "', '" + 
                        publisher_qos.partition.name.get(1) + "'...\n");
                publisher.set_qos(publisher_qos);
            }
            else if ((samplesWritten+1) % 20 == 0) {
                // Strings that are regular expressions aren't tested for
                // literal matches, so this won't match "X*Z"
                publisher_qos.partition.name.set(0, "X*Z");
                System.out.print("Setting partition to '" + 
                        publisher_qos.partition.name.get(0) + "', '" + 
                        publisher_qos.partition.name.get(1) + "'...\n");
                publisher.set_qos(publisher_qos);		    
            }
            else if ((samplesWritten+1) % 15 == 0) {
                // Matches "ABC"
                publisher_qos.partition.name.set(0, "A?C");
                System.out.print("Setting partition to '" + 
                        publisher_qos.partition.name.get(0) + "', '" + 
                        publisher_qos.partition.name.get(1) + "'...\n");
                publisher.set_qos(publisher_qos);
            }
            else if ((samplesWritten+1) % 10 == 0) {
                // Matches "ABC"
                publisher_qos.partition.name.set(0, "A*");
                System.out.print("Setting partition to '" + 
                        publisher_qos.partition.name.get(0) + "', '" + 
                        publisher_qos.partition.name.get(1) + "'...\n");
                publisher.set_qos(publisher_qos);
            }
            else if ((samplesWritten+1) % 5 == 0) {
                // No literal match for "bar"
                publisher_qos.partition.name.set(0, "bar");
                System.out.print("Setting partition to '" +
                        publisher_qos.partition.name.get(0) + "', '" + 
                        publisher_qos.partition.name.get(1) + "'...\n");
                publisher.set_qos(publisher_qos);
            }
        }

        //writer.unregister_instance(data, instance_handle);
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
        try (partitionsPublisher publisherApplication = new partitionsPublisher()) {
            publisherApplication.parseArguments(args);
            publisherApplication.addShutdownHook();
            publisherApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}

        