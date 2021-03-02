/*
* (c) Copyright, Real-Time Innovations, 2021.  All rights reserved.
* RTI grants Licensee a license to use, modify, compile, and create derivative
* works of the software solely for use with RTI Connext DDS. Licensee may
* redistribute copies of the software provided that all such copies are subject
* to this license. The software is provided "as is", with no warranty of any
* type, including any warranty for fitness for any purpose. RTI is under no
* obligation to maintain or support the software. RTI shall not be liable for
* any incidental or consequential damages arising out of the use or inability
* to use the software.
*/

import java.util.Objects;

import com.rti.dds.domain.DomainParticipant;
import com.rti.dds.domain.DomainParticipantFactory;
import com.rti.dds.infrastructure.InstanceHandle_t;
import com.rti.dds.infrastructure.StatusKind;
import com.rti.dds.publication.Publisher;
import com.rti.dds.topic.Topic;
import com.rti.ndds.config.*;
import com.rti.dds.infrastructure.Time_t;

/** 
* Simple example showing all Connext code in one place for readability.
*/
public class loggingPublisher extends Application implements AutoCloseable {

    // Usually one per application
    private DomainParticipant participant = null;

    private void runApplication() {
        // Start communicating in a domain
        participant = Objects.requireNonNull(
            DomainParticipantFactory.get_instance().create_participant(
                getDomainId(),
                DomainParticipantFactory.PARTICIPANT_QOS_DEFAULT,
                null, // listener
                StatusKind.STATUS_MASK_NONE));


        /*
         * Start - modifying the generated example to showcase the usage of
         * the Logging API.
         */

        /*
         * Set NDDS_CONFIG_LOG_VERBOSITY_WARNING NDDS_Config_LogVerbosity.
         * Set NDDS_CONFIG_LOG_PRINT_FORMAT_MAXIMAL NDDS_Config_LogPrintFormat
         * for NDDS_CONFIG_LOG_LEVEL_WARNING.
         */
        Logger logger = Objects.requireNonNull(Logger.get_instance());
        logger.set_verbosity(LogVerbosity.NDDS_CONFIG_LOG_VERBOSITY_WARNING);
        logger.set_print_format_by_log_level(
                LogPrintFormat.NDDS_CONFIG_LOG_PRINT_FORMAT_MAXIMAL,
                LogLevel.NDDS_CONFIG_LOG_LEVEL_WARNING);

        // A Publisher allows an application to create one or more DataWriters
        Publisher publisher = Objects.requireNonNull(
            participant.create_publisher(
                DomainParticipant.PUBLISHER_QOS_DEFAULT,
                null, // listener
                StatusKind.STATUS_MASK_NONE));

        // Register the datatype to use when creating the Topic
        String typeName = loggingTypeSupport.get_type_name();
        loggingTypeSupport.register_type(participant, typeName);

        // Create a Topic with a name and a datatype
        Topic topic = Objects.requireNonNull(
            participant.create_topic(
                "Example logging",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null, // listener
                StatusKind.STATUS_MASK_NONE));

        // This DataWriter writes data on "Example logging" Topic
        loggingDataWriter writer = (loggingDataWriter) Objects.requireNonNull(
            publisher.create_datawriter(
                topic,
                Publisher.DATAWRITER_QOS_DEFAULT,
                null, // listener
                StatusKind.STATUS_MASK_NONE));

        // Create data sample for writing
        logging data = new logging();


        writer.write(data, InstanceHandle_t.HANDLE_NIL);
        /*
        * Force a warning by writing a sample where the source time stamp is older
        * than that of a previously sent sample. When using
        * DDS_BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS: If source timestamp is
        * older than in previous write a warnings message will be logged.
        */
        writer.write_w_timestamp(data, InstanceHandle_t.HANDLE_NIL, new Time_t(0,0));
        /*
        * End - modifying the generated example to showcase the usage of
        * the Logging API.
        */
    }

    @Override
    public void close() {
        // Delete all entities (DataWriter, Topic, Publisher, DomainParticipant)
        if (participant != null) {
            participant.delete_contained_entities();

            DomainParticipantFactory.get_instance()
            .delete_participant(participant);
        }
    }

    public static void main(String[] args) {
        // Create example and run: Uses try-with-resources,
        // publisherApplication.close() automatically called
        try (loggingPublisher publisherApplication = new loggingPublisher()) {
            publisherApplication.parseArguments(args);
            publisherApplication.addShutdownHook();
            publisherApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at application exit.
        DomainParticipantFactory.finalize_instance();
    }
}