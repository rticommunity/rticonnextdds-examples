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
import com.rti.dds.dynamicdata.DynamicData;
import com.rti.dds.dynamicdata.DynamicDataTypeSupport;
import com.rti.dds.dynamicdata.DynamicDataWriter;
import com.rti.dds.infrastructure.InstanceHandle_t;
import com.rti.dds.infrastructure.StatusKind;
import com.rti.dds.publication.Publisher;
import com.rti.dds.topic.Topic;
import com.rti.dds.typecode.TypeCode;

// ===========================================================================

public class ShapeTypePublisher extends Application implements AutoCloseable {
    private static String EXAMPLE_TYPE_NAME = "ShapesType";

    // Usually one per application
    private DomainParticipant participant = null;

    private void runApplication()
    {
        /*** Shape direction variables ***/
        int direction = 1;   /* 1 means left to right and -1 right to left */
        int x_position = 50; /* 50 is the initial position */

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

        /*
         * Create DynamicData using TypeCode from ShapeTypeTypeCode If you
         * are NOT using a type generated with rtiddsgen, you need to create
         * this TypeCode from scratch.
         */
        TypeCode type_code = ShapeTypeTypeCode.VALUE;

        // Create the Dynamic data type support object
        DynamicDataTypeSupport type_support = new DynamicDataTypeSupport(
                type_code,
                DynamicDataTypeSupport.TYPE_PROPERTY_DEFAULT);

        // --- Create topic --- //

        // Register type before creating topic
        String typeName = EXAMPLE_TYPE_NAME;
        type_support.register_type(participant, typeName);

        /*
         * Make sure both publisher and subscriber share the same topic
         * name. In the Shapes example: we are publishing a Square, which is
         * the topic name. If you want to publish other shapes (Triangle or
         * Circle), you just need to update the topic name.
         */
        Topic topic = Objects.requireNonNull(participant.create_topic(
                "Square",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));

        // --- Create writer --- //
        /* To use DynamicData, we need to assign the generic
         * DataWriter to a DynamicDataWriter, using a casting.
         */
        DynamicDataWriter writer = (DynamicDataWriter) Objects.requireNonNull(
                participant.create_datawriter(
                        topic,
                        Publisher.DATAWRITER_QOS_DEFAULT,
                        null, /* listener */
                        StatusKind.STATUS_MASK_NONE));

        // Create an instance of the sparse data we are about to send
        DynamicData data =
                new DynamicData(type_code, DynamicData.PROPERTY_DEFAULT);

        // Initialize the DynamicData object
        data.set_string("color", DynamicData.MEMBER_ID_UNSPECIFIED, "BLUE");
        data.set_int("x", DynamicData.MEMBER_ID_UNSPECIFIED, 100);
        data.set_int("y", DynamicData.MEMBER_ID_UNSPECIFIED, 100);
        data.set_int("shapesize", DynamicData.MEMBER_ID_UNSPECIFIED, 30);

        final long sendPeriodMillis = 100;  // 100 ms

        for (int samplesWritten = 0;
             !isShutdownRequested() && samplesWritten < getMaxSampleCount();
             samplesWritten++) {
            System.out.println(
                    "Sending shapesize " + (30 + (samplesWritten % 20)));
            System.out.println("Sending x position " + x_position);

            /* Modify the shapesize from 30 to 50 */
            data.set_int(
                    "shapesize",
                    DynamicData.MEMBER_ID_UNSPECIFIED,
                    30 + (samplesWritten % 20));

            /* Modify the position */
            data.set_int("x", DynamicData.MEMBER_ID_UNSPECIFIED, x_position);

            /*
             * The x_position will be modified adding or substracting 2 to
             * the previous x_position depending on the direction.
             */
            x_position += (direction * 2);

            /*
             * The x_position will stay between 50 and 150 pixels. When the
             * position is greater than 150 'direction' will be negative
             * (moving to the left) and when it is lower than 50 'direction'
             * will be possitive (moving to the right).
             */
            if (x_position >= 150) {
                direction = -1;
            }
            if (x_position <= 50) {
                direction = 1;
            }

            /* Write data */
            writer.write(data, InstanceHandle_t.HANDLE_NIL);
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
        try (ShapeTypePublisher publisherApplication =
                     new ShapeTypePublisher()) {
            publisherApplication.parseArguments(args);
            publisherApplication.addShutdownHook();
            publisherApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
