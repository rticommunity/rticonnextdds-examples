/*******************************************************************************
 (c) 2005-2015 Copyright, Real-Time Innovations, Inc.  All rights reserved.
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

public class FlightPublisher extends Application implements AutoCloseable {
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

        // Register type before creating topic
        String typeName = FlightTypeSupport.get_type_name();
        FlightTypeSupport.register_type(participant, typeName);

        // Create a Topic with a name and a datatype
        Topic topic = Objects.requireNonNull(participant.create_topic(
                "Example Flight",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));

        // This DataWriter writes data on "Example Flight" Topic
        FlightDataWriter writer = (FlightDataWriter) Objects.requireNonNull(
                publisher.create_datawriter(
                        topic,
                        Publisher.DATAWRITER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        // Create data sample for writing.
        Flight[] flights_info = new Flight[4];
        flights_info[0] = new Flight();
        flights_info[0].trackId = 1111;
        flights_info[0].company = "CompanyA";
        flights_info[0].altitude = 15000;
        flights_info[1] = new Flight();
        flights_info[1].trackId = 2222;
        flights_info[1].company = "CompanyB";
        flights_info[1].altitude = 20000;
        flights_info[2] = new Flight();
        flights_info[2].trackId = 3333;
        flights_info[2].company = "CompanyA";
        flights_info[2].altitude = 30000;
        flights_info[3] = new Flight();
        flights_info[3].trackId = 4444;
        flights_info[3].company = "CompanyB";
        flights_info[3].altitude = 25000;

        final long sendPeriodMillis = 1000;  // 1 second.

        for (int samplesWritten = 0;
             !isShutdownRequested() && samplesWritten < getMaxSampleCount();
             samplesWritten++) {
            // Update flight info latitude and write.
            System.out.println("Updating and sending values");
            for (int i = 0; i < flights_info.length; i++) {
                // Set the plane altitude lineally (usually the max is
                // at 41,000ft).
                int altitude = flights_info[i].altitude + samplesWritten * 100;
                flights_info[i].altitude = altitude >= 41000 ? 41000 : altitude;

                System.out.format(
                        "\t[trackId: %d, company: %s, altitude: %d]\n",
                        flights_info[i].trackId,
                        flights_info[i].company,
                        flights_info[i].altitude);

                writer.write(flights_info[i], InstanceHandle_t.HANDLE_NIL);
            }

            try {
                Thread.sleep(sendPeriodMillis);
            } catch (InterruptedException ix) {
                System.err.println("INTERRUPTED");
                break;
            }
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
        try (FlightPublisher publisherApplication = new FlightPublisher()) {
            publisherApplication.parseArguments(args);
            publisherApplication.addShutdownHook();
            publisherApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
