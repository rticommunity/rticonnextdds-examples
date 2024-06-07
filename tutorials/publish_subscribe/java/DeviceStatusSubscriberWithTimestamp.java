/*
 * (c) 2024 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 *  RTI grants Licensee a license to use, modify, compile, and create derivative
 *  works of the Software.  Licensee has the right to distribute object form
 *  only for use with RTI products.  The Software is provided "as is", with no
 *  warranty of any type, including any warranty for fitness for any purpose.
 *  RTI is under no obligation to maintain or support the Software.  RTI shall
 *  not be liable for any incidental or consequential damages arising out of the
 *  use or inability to use the software.
 */

import java.util.Objects;
import java.lang.reflect.*;

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


public class DeviceStatusSubscriberWithTimestamp extends Application implements AutoCloseable {

    private DomainParticipant participant = null;
    private DeviceStatusDataReader reader = null;
    private final DeviceStatusSeq dataSeq = new DeviceStatusSeq();
    private final SampleInfoSeq infoSeq = new SampleInfoSeq();

    private int processData() {
        int samplesRead = 0;

        try {
            reader.take(dataSeq,
                    infoSeq,
                    ResourceLimitsQosPolicy.LENGTH_UNLIMITED,
                    SampleStateKind.ANY_SAMPLE_STATE,
                    ViewStateKind.ANY_VIEW_STATE,
                    InstanceStateKind.ANY_INSTANCE_STATE);

            for (int i = 0; i < dataSeq.size(); ++i) {
                SampleInfo info = infoSeq.get(i);

                if (info.valid_data) {
                    DeviceStatus deviceStatus = dataSeq.get(i);
                    if (deviceStatus.is_open) {
                        double timestamp = info.source_timestamp.sec + (info.source_timestamp.nanosec / 1e9);
                        System.out.println("WARNING: " + deviceStatus.sensor_name
                                + " in " + deviceStatus.room_name + " is open ("
                                + String.format( "%.2f", timestamp ) + " s)");
                    }

                }
                samplesRead++;
            }
        } catch (RETCODE_NO_DATA noData) {
            // No data to process, not a problem
        } finally {
            reader.return_loan(dataSeq, infoSeq);
        }

        return samplesRead;
    }

    private void runApplication() {
        participant = Objects.requireNonNull(
            DomainParticipantFactory.get_instance().create_participant(
                0,
                DomainParticipantFactory.PARTICIPANT_QOS_DEFAULT,
                null, // listener
                StatusKind.STATUS_MASK_NONE));

        String typeName = DeviceStatusTypeSupport.get_type_name();
        DeviceStatusTypeSupport.register_type(participant, typeName);

        Topic topic = Objects.requireNonNull(
            participant.create_topic(
                "WindowStatus",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null, // listener
                StatusKind.STATUS_MASK_NONE));

        reader = (DeviceStatusDataReader) Objects.requireNonNull(
            participant.create_datareader(
                topic,
                Subscriber.DATAREADER_QOS_DEFAULT,
                null, // listener
                StatusKind.STATUS_MASK_NONE));

        ReadCondition condition = reader.create_readcondition(
                SampleStateKind.ANY_SAMPLE_STATE,
                ViewStateKind.ANY_VIEW_STATE,
                InstanceStateKind.ANY_INSTANCE_STATE);

        WaitSet waitset = new WaitSet();
        waitset.attach_condition(condition);
        final Duration_t waitTimeout = new Duration_t(2, 0);

        int samplesRead = 0;
        ConditionSeq activeConditions = new ConditionSeq();

        while (!isShutdownRequested() && samplesRead < 1000) {
            try {
                waitset.wait(activeConditions, waitTimeout);

                samplesRead += processData();
            } catch (RETCODE_TIMEOUT timeout) {
                // No data, received
            }
        }
    }

    @Override
    public void close() {
        if (participant != null) {
            participant.delete_contained_entities();

            DomainParticipantFactory.get_instance()
            .delete_participant(participant);
        }
    }

    public static void main(String[] args) {

        try (DeviceStatusSubscriberWithTimestamp subscriberApplication =
                new DeviceStatusSubscriberWithTimestamp()) {
            subscriberApplication.addShutdownHook();
            subscriberApplication.runApplication();
        }

        DomainParticipantFactory.finalize_instance();
    }
}
