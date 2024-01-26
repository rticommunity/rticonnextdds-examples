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


public class keysSubscriber extends Application implements AutoCloseable {
    private DomainParticipant participant = null;  // Usually one per
                                                   // application
    private keysDataReader reader = null;
    private final keysSeq dataSeq = new keysSeq();
    private final SampleInfoSeq infoSeq = new SampleInfoSeq();

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

                // Start changes for Keyed_Data
                // We first check if the sample includes valid data
                if (info.valid_data) {
                    keys sample = (keys) dataSeq.get(i);
                    if (info.view_state == ViewStateKind.NEW_VIEW_STATE) {
                        System.out.print(
                                "Found new instance; code = " + sample.code
                                + "\n");
                    }
                    System.out.print(
                            "Instance " + sample.code + ": x: " + sample.x
                            + ", y: " + sample.y + "\n");
                } else {
                    keys dummy = new keys();
                    reader.get_key_value(dummy, info.instance_handle);
                    if (info.instance_state
                        == InstanceStateKind
                                   .NOT_ALIVE_NO_WRITERS_INSTANCE_STATE) {
                        System.out.print(
                                "Instance " + dummy.code + " has no writers\n");
                    } else if (
                            info.instance_state
                            == InstanceStateKind
                                       .NOT_ALIVE_DISPOSED_INSTANCE_STATE) {
                        System.out.print(
                                "Instance " + dummy.code + " is disposed\n");
                    } else {
                        System.out.print(
                                "Instance " + dummy.code + " is alive\n");
                    }
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

        // A Subscriber allows an application to create one or more DataReaders
        Subscriber subscriber =
                Objects.requireNonNull(participant.create_subscriber(
                        DomainParticipant.SUBSCRIBER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        // Register the datatype to use when creating the Topic
        String typeName = keysTypeSupport.get_type_name();
        keysTypeSupport.register_type(participant, typeName);

        // Create a Topic with a name and a datatype
        Topic topic = Objects.requireNonNull(participant.create_topic(
                "Example keys",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));

        // This DataReader reads data on "Example keys" Topic
        reader = (keysDataReader) Objects.requireNonNull(
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
        final Duration_t waitTimeout = new Duration_t(1, 0);

        int samplesRead = 0;
        ConditionSeq activeConditions = new ConditionSeq();

        // Main loop. Wait for data to arrive and process when it arrives
        while (!isShutdownRequested() && samplesRead < getMaxSampleCount()) {
            // System.out.println("keys subscriber sleeping for " +
            // receivePeriodSec + " sec...");
            try {
                // Wait fills in activeConditions or times out
                waitset.wait(activeConditions, waitTimeout);

                // Read condition triggered, process data
                samplesRead += processData();

            } catch (RETCODE_TIMEOUT timeout) {
                // No data received, not a problem
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
        try (keysSubscriber subscriberApplication = new keysSubscriber()) {
            subscriberApplication.parseArguments(args);
            subscriberApplication.addShutdownHook();
            subscriberApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
