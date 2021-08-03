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

import java.text.DecimalFormat;
import java.util.Objects;

import com.rti.dds.domain.DomainParticipant;
import com.rti.dds.domain.DomainParticipantFactory;
import com.rti.dds.infrastructure.ConditionSeq;
import com.rti.dds.infrastructure.Duration_t;
import com.rti.dds.infrastructure.RETCODE_NO_DATA;
import com.rti.dds.infrastructure.RETCODE_TIMEOUT;
import com.rti.dds.infrastructure.ResourceLimitsQosPolicy;
import com.rti.dds.infrastructure.StatusKind;
import com.rti.dds.infrastructure.WaitSet;
import com.rti.dds.subscription.InstanceStateKind;
import com.rti.dds.subscription.ReadCondition;
import com.rti.dds.subscription.SampleInfo;
import com.rti.dds.subscription.SampleInfoSeq;
import com.rti.dds.subscription.SampleStateKind;
import com.rti.dds.subscription.Subscriber;
import com.rti.dds.subscription.ViewStateKind;
import com.rti.dds.topic.Topic;


public class cfcSubscriber extends Application implements AutoCloseable {
    private DomainParticipant participant = null;  // Usually one per
                                                   // application
    private cfcDataReader reader = null;
    private final cfcSeq dataSeq = new cfcSeq();
    private final SampleInfoSeq infoSeq = new SampleInfoSeq();

    long initms = System.currentTimeMillis();
    DecimalFormat decFormat = new DecimalFormat("#.##");

    private int processData()
    {
        int samplesRead = 0;

        try {
            // Take available data from DataReader's queue
            reader.take(
                    dataSeq,
                    infoSeq,
                    ResourceLimitsQosPolicy.LENGTH_UNLIMITED,
                    SampleStateKind.ANY_SAMPLE_STATE,
                    ViewStateKind.ANY_VIEW_STATE,
                    InstanceStateKind.ANY_INSTANCE_STATE);

            for (int i = 0; i < dataSeq.size(); ++i) {
                SampleInfo info = infoSeq.get(i);
                if (info.valid_data) {
                    double elapsed =
                            (System.currentTimeMillis() - initms) / 1000.0;
                    System.out.println(
                            "@ t=" + decFormat.format(elapsed)
                            + ", got x = " + ((cfc) dataSeq.get(i)).x);
                }
                samplesRead++;
            }
        } catch (RETCODE_NO_DATA noData) {
            // No data to process, not a problem
        } finally {
            // Data loaned from Connext for performance. Return loan when done.
            reader.return_loan(dataSeq, infoSeq);
        }

        return samplesRead;
    }

    private void runApplication()
    {
        // Start communicating in a domain
        participant = Objects.requireNonNull(
                DomainParticipantFactory.get_instance().create_participant(
                        getDomainId(),
                        DomainParticipantFactory.PARTICIPANT_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        /* Start changes for Custom_Flowcontroller */

        /*
         * If you want to change the Participant's QoS programmatically
         * rather than using the XML file, you will need to add the
         * following lines to your code and comment out the
         * create_participant call above.
         */

        /*
         * By default, discovery will communicate via shared memory for
         * platforms that support it. Because we disable shared memory on
         * the publishing side, we do so here to ensure the reader and
         * writer discover each other.
         */

        /* Get default participant QoS to customize */
        /*
            DomainParticipantQos participant_qos = new DomainParticipantQos();
            DomainParticipantFactory.TheParticipantFactory
                    .get_default_participant_qos(participant_qos);

            // By default, data will be sent via shared memory _and_ UDPv4.
            // Because the flowcontroller limits writes across all interfaces,
            // this halves the effective send rate. To avoid this, we enable
            // only the UDPv4 transport
            participant_qos.transport_builtin.mask = TransportBuiltinKind.UDPv4;

            // To create participant with default QoS, use
            // DomainParticipantFactory.PARTICIPANT_QOS_DEFAULT
            // instead of participant_qos
            participant = Objects.requireNonNull(
                DomainParticipantFactory.get_instance().create_participant(
                        getDomainId(),
                        participant_qos,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));
        */
        /* End changes for Custom_Flowcontroller */

        // A Subscriber allows an application to create one or more DataReaders
        Subscriber subscriber =
                Objects.requireNonNull(participant.create_subscriber(
                        DomainParticipant.SUBSCRIBER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        // Register type before creating topic
        String typeName = cfcTypeSupport.get_type_name();
        cfcTypeSupport.register_type(participant, typeName);

        // Create a Topic with a name and a datatype
        Topic topic = Objects.requireNonNull(participant.create_topic(
                "Example cfc",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));


        // This DataReader reads data on "Example cfc" Topic
        reader = (cfcDataReader) Objects.requireNonNull(
                subscriber.create_datareader(
                        topic,
                        Subscriber.DATAREADER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        // Create ReadCondition that triggers when data in reader's queue
        ReadCondition condition = reader.create_readcondition(
                SampleStateKind.ANY_SAMPLE_STATE,
                ViewStateKind.ANY_VIEW_STATE,
                InstanceStateKind.ANY_INSTANCE_STATE);

        // WaitSet will be woken when the attached condition is triggered, or
        // timeout
        WaitSet waitset = new WaitSet();
        waitset.attach_condition(condition);
        final Duration_t waitTimeout = new Duration_t(4, 0);

        int samplesRead = 0;
        ConditionSeq activeConditions = new ConditionSeq();

        // Main loop. Wait for data to arrive and process when it arrives
        while (!isShutdownRequested() && samplesRead < getMaxSampleCount()) {
            try {
                // Wait fills in activeConditions or times out
                waitset.wait(activeConditions, waitTimeout);

                // Read condition triggered, process data
                samplesRead += processData();

            } catch (RETCODE_TIMEOUT timeout) {
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
        try (cfcSubscriber subscriberApplication = new cfcSubscriber()) {
            subscriberApplication.parseArguments(args);
            subscriberApplication.addShutdownHook();
            subscriberApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
