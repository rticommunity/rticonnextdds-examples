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
import java.util.Random;

import com.rti.dds.domain.DomainParticipant;
import com.rti.dds.domain.DomainParticipantFactory;
import com.rti.dds.infrastructure.InstanceHandle_t;
import com.rti.dds.infrastructure.StatusKind;
import com.rti.dds.publication.Publisher;
import com.rti.dds.topic.Topic;


public class sequencesPublisher extends Application implements AutoCloseable {
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
        String typeName = sequencesTypeSupport.get_type_name();
        sequencesTypeSupport.register_type(participant, typeName);

        // Create a Topic with a name and a datatype
        Topic topic = Objects.requireNonNull(participant.create_topic(
                "Example sequences",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));

        // This DataWriter writes data on "Example sequences" Topic
        sequencesDataWriter writer =
                (sequencesDataWriter) Objects.requireNonNull(
                        publisher.create_datawriter(
                                topic,
                                Publisher.DATAWRITER_QOS_DEFAULT,
                                null,  // listener
                                StatusKind.STATUS_MASK_NONE));

        /* Here we define two instances of sequences: ownerInstance and
         * borrowerInstance.
         */

        /* ownerInstance.data uses its own memory as, by default, a sequence
         * you create owns its memory unless you explicitly loan memory of
         * your own to it.
         */
        sequences ownerInstance = new sequences();
        sequences borrowerInstance = new sequences();

        InstanceHandle_t ownerInstanceHandle = InstanceHandle_t.HANDLE_NIL;
        InstanceHandle_t borrowerInstanceHandle = InstanceHandle_t.HANDLE_NIL;

        /* If we want borrower_instance.data to loan a buffer of shorts,
         * first we have to allocate the buffer. Here we allocate a buffer
         * of MAX_SEQUENCE_LEN.
         */
        short[] shortBuffer = new short[MAX_SEQUENCE_LEN.VALUE];

        /* Before calling loan(), we need to set sequence maximum to 0,
         * i.e., the sequence won't have memory allocated to it.
         */
        borrowerInstance.data.setMaximum(0);

        /* Now that the sequence doesn't have memory allocated to it, we can
         * call loan() to loan shortBuffer to borrowerInstance.
         * We set the allocated number of elements to MAX_SEQUENCE_LEN, the
         * size of the buffer and the maximum size of the sequence as we
         * declared in the IDL. */
        borrowerInstance.data.loan(
                shortBuffer,  // Buffer
                0             // Initial Length
        );

        /* Before starting to publish samples, set the instance id of each
         * instance
         */
        ownerInstance.id = "owner_instance";
        borrowerInstance.id = "browser_instance";

        // Send a new sample every second
        final long sendPeriodMillis = 1 * 1000;  // 1 second

        // We use Random to generate random values for the sequences
        Random rand = new Random();

        /* To illustrate the use of the sequences, in the main loop we set a
         * new sequence length every iteration to the sequences contained in
         * both instances (instance.data). The sequence length value cycles
         * between 0 and MAX_SEQUENCE_LEN. We assign a random number between
         * 0 and 100 to each sequence's elements.
         */
        for (int samplesWritten = 0;
             !isShutdownRequested() && samplesWritten < getMaxSampleCount();
             samplesWritten++) {
            /* We set a different sequenceLength for both instances every
             * iteration. sequenceLength is based on the value of count
             * and its value cycles between the values of 1 and
             * MAX_SEQUENCE_LEN. */
            int sequenceLength = (samplesWritten % MAX_SEQUENCE_LEN.VALUE) + 1;

            System.out.println("Writing sequences, count " + samplesWritten);

            ownerInstance.count = (short) samplesWritten;
            borrowerInstance.count = (short) samplesWritten;

            // Here we set the new length of each sequence
            ownerInstance.data.setSize(sequenceLength);
            borrowerInstance.data.setSize(sequenceLength);

            /* Now that the sequences have a new length, we assign a
             * random number between 0 and 100 to each element of
             * ownerInstance.data and borrowerInstance.data.
             */
            for (int i = 0; i < sequenceLength; ++i) {
                ownerInstance.data.setShort(i, (short) rand.nextInt(100));
                borrowerInstance.data.setShort(i, (short) rand.nextInt(100));
            }

            /* Both sequences have the same length, so we only print the
             * length of one of them.
             */
            System.out.println(
                    "Instances length = " + ownerInstance.data.size());

            // Write data
            writer.write(ownerInstance, ownerInstanceHandle);
            try {
                Thread.sleep(sendPeriodMillis);
            } catch (InterruptedException ix) {
                System.err.println("INTERRUPTED");
                break;
            }

            writer.write(borrowerInstance, borrowerInstanceHandle);
            try {
                Thread.sleep(sendPeriodMillis);
            } catch (InterruptedException ix) {
                System.err.println("INTERRUPTED");
                break;
            }
        }

        // Once we are done with the sequence, we call unloan()
        borrowerInstance.data.unloan();
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
        try (sequencesPublisher publisherApplication =
                     new sequencesPublisher()) {
            publisherApplication.parseArguments(args);
            publisherApplication.addShutdownHook();
            publisherApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
