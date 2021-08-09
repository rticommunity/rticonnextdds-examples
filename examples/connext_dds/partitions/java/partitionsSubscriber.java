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
import com.rti.dds.subscription.SubscriberQos;
import com.rti.dds.subscription.ViewStateKind;
import com.rti.dds.topic.Topic;

public class partitionsSubscriber extends Application implements AutoCloseable {
    private DomainParticipant participant = null;  // Usually one per
                                                   // application
    private partitionsDataReader reader = null;
    private final partitionsSeq dataSeq = new partitionsSeq();
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

            for(int i = 0; i < dataSeq.size(); ++i) {
                SampleInfo info = (SampleInfo) infoSeq.get(i);
                
                if (info.valid_data) {
                    if (info.view_state == ViewStateKind.NEW_VIEW_STATE) {
                        System.out.print("Found new instance\n");
                    }
                    System.out.println("   x: " + 
                        ((partitions) dataSeq.get(i)).x);
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
    
    private void runApplication() {

        // Start communicating in a domain
        participant = Objects.requireNonNull(
                DomainParticipantFactory.get_instance().create_participant(
                        getDomainId(),
                        DomainParticipantFactory.PARTICIPANT_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

	    SubscriberQos subscriber_qos = new SubscriberQos();
        participant.get_default_subscriber_qos(subscriber_qos);
	    
	    /* If you want to change the Partition name programmatically rather than
	     * using the XML, you will need to add the following lines to your code
	     * and comment out the create_subscriber() call bellow.
	     */
	    /*
        subscriber_qos.partition.name.clear();
        subscriber_qos.partition.name.add("ABC");
        subscriber_qos.partition.name.add("X*Z");

	    Subscriber subscriber = Subscriber subscriber =
                Objects.requireNonNull(participant.create_subscriber(
                        subscriber_qos,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));
	    */
    
        // A Subscriber allows an application to create one or more DataReaders
        Subscriber subscriber =
                Objects.requireNonNull(participant.create_subscriber(
                        DomainParticipant.SUBSCRIBER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));
	    
        System.out.print("Setting partition to '" +
                subscriber_qos.partition.name.get(0) + "', '" +
                subscriber_qos.partition.name.get(1) + "'...\n");

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

	    /* If you want to change the Datareader QoS programmatically rather than
	     * using the XML, you will need to add the following lines to your code
	     * and comment out the create_datareader() call bellow.
	     */
	    
	    /*
	    DataReaderQos datareader_qos = new DataReaderQos();
            subscriber.get_default_datareader_qos(datareader_qos);

            datareader_qos.reliability.kind =
                ReliabilityQosPolicyKind.RELIABLE_RELIABILITY_QOS;
            datareader_qos.durability.kind = 
                DurabilityQosPolicyKind.TRANSIENT_LOCAL_DURABILITY_QOS;
            datareader_qos.history.kind = 
                HistoryQosPolicyKind.KEEP_ALL_HISTORY_QOS;
	    
	    partitionsDataReader reader = (partitionsDataReader) Objects.requireNonNull(
                subscriber.create_datareader(
                        topic,
                        datareader_qos,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));
	    */

        // This DataReader reads data on "Example partitions" Topic
        reader = (partitionsDataReader) Objects.requireNonNull(
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
        try (partitionsSubscriber subscriberApplication = new partitionsSubscriber()) {
            subscriberApplication.parseArguments(args);
            subscriberApplication.addShutdownHook();
            subscriberApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
