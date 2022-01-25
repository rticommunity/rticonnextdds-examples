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
import com.rti.dds.infrastructure.InstanceHandle_t;
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

    /* *** Start changes for Advanced_Keys *** */
    /* Track instance state */
    protected static final int State_Inactive = 0;
    protected static final int State_Active = 1;
    protected static final int State_No_Writers = 2;
    protected static final int State_Disposed = 3;
    protected int states[] = { State_Inactive, State_Inactive, State_Inactive };

    private int processData()
    {
        int samplesRead = 0;

        while (true) {
            /* Given DDS_HANDLE_NIL as a parameter, take_next_instance returns
             * a sequence containing samples from only the next (in a
             * well-determined but unspecified order) un-taken instance.
             */
            try {
                reader.take_next_instance(
                        dataSeq,
                        infoSeq,
                        ResourceLimitsQosPolicy.LENGTH_UNLIMITED,
                        InstanceHandle_t.HANDLE_NIL,
                        SampleStateKind.ANY_SAMPLE_STATE,
                        ViewStateKind.ANY_VIEW_STATE,
                        InstanceStateKind.ANY_INSTANCE_STATE);

                /* We process all the obtained samples for a particular instance
                 */
                for (int i = 0; i < infoSeq.size(); ++i) {
                    SampleInfo info = (SampleInfo) infoSeq.get(i);
                    /* We first check if the sample includes valid data */
                    if (info.valid_data) {
                        keys data = (keys) dataSeq.get(i);

                        if (info.view_state == ViewStateKind.NEW_VIEW_STATE) {
                            new_instance_found(reader, info, data);
                        }

                        /* We check if the obtained samples are associated to
                        one of the instances of interest. Since
                        take_next_instance gives sequences of the same instance,
                        we only need to test this for the first sample obtained.
                         */
                        if (i == 0 && !key_is_relevant(data)) {
                            break;
                        }

                        handle_data(reader, info, data);
                    } else {
                        /* Since there is not valid data, it may include
                         * metadata */
                        keys dummy = new keys();
                        reader.get_key_value(dummy, info.instance_handle);

                        /* Here we print a message and change the instance state
                        if the instance state is ALIVE_NO_WRITERS or
                        ALIVE_DISPOSED */
                        if (info.instance_state
                            == InstanceStateKind
                                       .NOT_ALIVE_NO_WRITERS_INSTANCE_STATE) {
                            instance_lost_writers(reader, info, dummy);
                        } else if (
                                info.instance_state
                                == InstanceStateKind
                                           .NOT_ALIVE_DISPOSED_INSTANCE_STATE) {
                            instance_disposed(reader, info, dummy);
                        }
                    }
                }
            } catch (RETCODE_NO_DATA noData) {
                // No data to process
                break;
            } finally {
                // Prepare sequences for next take_next_instance
                reader.return_loan(dataSeq, infoSeq);
            }
        }

        return samplesRead;
    }

    /* These are not called by DDS.   on_data_available() calls
     * the appropriate function when it gets updates about
     * an instances' status
     */
    public void new_instance_found(
            keysDataReader keys_reader,
            SampleInfo info,
            keys msg)
    {
        /* There are three cases here:
        1.) truly new instance
        2.) instance lost all writers, but now we're getting data again
        3.) instance was disposed, but a new one has been created

        We distinguish these cases by examining generation counts, BUT
        note that if the instance resources have been reclaimed, the
        generation counts may be reset to 0.

        Instances are eligible for resource cleanup if there are no
        active writers and all samples have been taken.  To reliably
        determine which case a 'new' instance falls into, the application
        must store state information on a per-instance basis.

        Note that this example assumes that state changes only occur via
        explicit register_instance(), unregister_instance() and dispose()
        calls from the datawriter.  In reality, these changes could also
        occur due to lost liveliness or missed deadlines, so those
        listeners would also need to update the instance state.
         */

        switch (states[msg.code]) {
        case State_Inactive:
            System.out.print("New instance found; code = " + msg.code + "\n");
            break;
        case State_Active:
            // An active instance should never be interpreted as new
            System.out.print(
                    "Error, 'new' already-active instance found; code = "
                    + msg.code + "\n");
            break;
        case State_No_Writers:
            System.out.print(
                    "Found writer for instance; code = " + msg.code + "\n");
            break;
        case State_Disposed:
            System.out.print(
                    "Found reborn instance; code = " + msg.code + "\n");
            break;
        }
        states[msg.code] = State_Active;
    }
    public void instance_lost_writers(
            keysDataReader keys_reader,
            SampleInfo info,
            keys msg)
    {
        System.out.print("Instance has no writers; code = " + msg.code + "\n");
        states[msg.code] = State_No_Writers;
    }

    public void instance_disposed(
            keysDataReader keys_reader,
            SampleInfo info,
            keys msg)
    {
        System.out.print("Instance disposed; code = " + msg.code + "\n");
        states[msg.code] = State_Disposed;
    }

    // Called to handle relevant data samples
    public void handle_data(
            keysDataReader keys_reader,
            SampleInfo info,
            keys msg)
    {
        System.out.print(
                "code: " + msg.code + ", x: " + msg.x + ", y: " + msg.y + "\n");
    }

    /* Called to determine if a key is relevant to this application */
    boolean key_is_relevant(keys msg)
    {
        /* For this example we just care about codes > 0,
         * which are the ones related to instances ins1 and ins2.
         */
        return (msg.code > 0);
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

        // Register type before creating topic
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

        /* If you want to set the QoS settings
         * programmatically rather than using the XML, you will need to add
         * the following lines to your code and comment out the
         * create_datareader call above.
         */

        /*
        DataReaderQos datareader_qos = new DataReaderQos();
        subscriber.get_default_datareader_qos(datareader_qos);

        datareader_qos.ownership.kind =
        OwnershipQosPolicyKind.EXCLUSIVE_OWNERSHIP_QOS;

        reader = (keysDataReader) Objects.requireNonNull(
                subscriber.create_datareader(
                        topic,
                        datareader_qos,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));
        */

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
