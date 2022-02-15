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
import com.rti.dds.domain.DomainParticipantQos;
import com.rti.dds.domain.DomainParticipantFactory;
import com.rti.dds.infrastructure.ConditionSeq;
import com.rti.dds.infrastructure.Duration_t;
import com.rti.dds.infrastructure.RETCODE_NO_DATA;
import com.rti.dds.infrastructure.RETCODE_TIMEOUT;
import com.rti.dds.infrastructure.ResourceLimitsQosPolicy;
import com.rti.dds.infrastructure.StatusKind;
import com.rti.dds.infrastructure.WaitSet;
import com.rti.dds.publication.builtin.PublicationBuiltinTopicData;
import com.rti.dds.publication.builtin.PublicationBuiltinTopicDataDataReader;
import com.rti.dds.publication.builtin.PublicationBuiltinTopicDataSeq;
import com.rti.dds.publication.builtin.PublicationBuiltinTopicDataTypeSupport;
import com.rti.dds.subscription.DataReader;
import com.rti.dds.subscription.DataReaderAdapter;
import com.rti.dds.subscription.InstanceStateKind;
import com.rti.dds.subscription.ReadCondition;
import com.rti.dds.subscription.SampleInfo;
import com.rti.dds.subscription.SampleInfoSeq;
import com.rti.dds.subscription.SampleStateKind;
import com.rti.dds.subscription.Subscriber;
import com.rti.dds.subscription.ViewStateKind;
import com.rti.dds.topic.Topic;

public class msgSubscriber extends Application implements AutoCloseable {
    // Usually one per application
    private DomainParticipant participant = null;
    private static int samplesRead = 0;

    private void runApplication()
    {
        // Start communicating in a domain
        participant = Objects.requireNonNull(
                DomainParticipantFactory.get_instance().create_participant(
                        getDomainId(),
                        DomainParticipantFactory.PARTICIPANT_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        /*
         * If you want to change the type_code_max_serialized_length
         * programmatically (e.g., to 3070) rather than using the XML file,
         * you will need to add the following lines to your code and comment
         * out the create_participant call above.
         */

        /*
        DomainParticipantQos participant_qos = new DomainParticipantQos();
        DomainParticipantFactory.TheParticipantFactory
                .get_default_participant_qos(participant_qos);

        participant_qos.resource_limits.type_code_max_serialized_length = 3070;

        participant = Objects.requireNonNull(
                DomainParticipantFactory.get_instance().create_participant(
                        getDomainId(),
                        participant_qos,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));
        */

        // First get the built-in subscriber
        Subscriber builtin_subscriber = participant.get_builtin_subscriber();

        // Then get the data reader for the built-in subscriber
        PublicationBuiltinTopicDataDataReader builtin_publication_datareader =
                (PublicationBuiltinTopicDataDataReader)
                        builtin_subscriber.lookup_datareader(
                                PublicationBuiltinTopicDataTypeSupport
                                        .PUBLICATION_TOPIC_NAME);

        // Finally install the listener
        BuiltinPublicationListener builtin_publication_listener =
                new BuiltinPublicationListener();

        builtin_publication_datareader.set_listener(
                builtin_publication_listener,
                StatusKind.DATA_AVAILABLE_STATUS);

        final long receivePeriodSec = 1;

        // Main loop. Wait for data to arrive and process when it arrives
        while (!isShutdownRequested() && samplesRead < getMaxSampleCount()) {
            try {
                Thread.sleep(receivePeriodSec * 1000);  // in millisec
            } catch (InterruptedException ix) {
                System.err.println("INTERRUPTED");
                break;
            }
        }
    }

    private static class BuiltinPublicationListener extends DataReaderAdapter {
        // Simple method to handle the participant key value

        private static String[] zero_pad = { "00000000", "0000000", "000000",
                                             "00000",    "0000",    "000",
                                             "00",       "0",       "" };

        public static String toHex(int i)
        {
            String s = Integer.toHexString(i);
            return (zero_pad[s.length()] + s);
        }

        public void on_data_available(DataReader reader)
        {
            PublicationBuiltinTopicDataSeq dataSeq =
                    new PublicationBuiltinTopicDataSeq();
            SampleInfoSeq infoSeq = new SampleInfoSeq();

            PublicationBuiltinTopicDataDataReader builtin_reader =
                    (PublicationBuiltinTopicDataDataReader) reader;

            try {
                builtin_reader.take(
                        dataSeq,
                        infoSeq,
                        ResourceLimitsQosPolicy.LENGTH_UNLIMITED,
                        SampleStateKind.ANY_SAMPLE_STATE,
                        ViewStateKind.ANY_VIEW_STATE,
                        InstanceStateKind.ANY_INSTANCE_STATE);

                for (int i = 0; i < infoSeq.size(); ++i) {
                    SampleInfo info = (SampleInfo) infoSeq.get(i);
                    PublicationBuiltinTopicData data =
                            (PublicationBuiltinTopicData) dataSeq.get(i);
                    if (!info.valid_data) {
                        continue;
                    }
                    System.out.printf(
                            "-----\nFound topic \"%s\"\n"
                                    + "participant: %08x%08x%08x\n"
                                    + "datawriter: %08x%08x%08x\ntype:\n",
                            data.topic_name,
                            data.participant_key.value[0],
                            data.participant_key.value[1],
                            data.participant_key.value[2],
                            data.key.value[0],
                            data.key.value[1],
                            data.key.value[2]);

                    if (data.type_code == null) {
                        System.out.print(
                                "No type code received, perhaps "
                                + "increase type_code_max_serialized_length?\n");
                        continue;
                    }

                    /*
                     * Using the type_code propagated we print the data type
                     * with print_IDL().
                     */
                    data.type_code.print_IDL(2);

                    samplesRead++;
                }
            } catch (RETCODE_NO_DATA noData) {
                // No data to process
            } finally {
                builtin_reader.return_loan(dataSeq, infoSeq);
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
        try (msgSubscriber subscriberApplication = new msgSubscriber()) {
            subscriberApplication.parseArguments(args);
            subscriberApplication.addShutdownHook();
            subscriberApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
