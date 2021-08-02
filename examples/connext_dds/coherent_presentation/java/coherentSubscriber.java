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


public class coherentSubscriber extends Application implements AutoCloseable {
    private DomainParticipant participant = null;  // Usually one per
                                                   // application
    private coherentDataReader reader = null;
    private final coherentSeq dataSeq = new coherentSeq();
    private final SampleInfoSeq infoSeq = new SampleInfoSeq();
    CoherentState reader_state = new CoherentState();

    private int processData()
    {
        int samplesRead = 0;

        try {
            reader.take(
                dataSeq, infoSeq,
                ResourceLimitsQosPolicy.LENGTH_UNLIMITED,
                SampleStateKind.ANY_SAMPLE_STATE,
                ViewStateKind.ANY_VIEW_STATE,
                InstanceStateKind.ANY_INSTANCE_STATE);
            // Start changes for Coherent_Presentation

            // Firstly process all samples
            int len = 0;
            coherent data;
            for(int i = 0; i < dataSeq.size(); ++i) {
                SampleInfo info = (SampleInfo) infoSeq.get(i);
                if (info.valid_data) {
                    data = (coherent) dataSeq.get(i);
                    len++;
                    reader_state.set_state(data.field, data.value);
                    samplesRead++;
                }
            }
            
            // Then, we print the results
            if (len > 0) {
                System.out.println("Received " + len + " updates\n  " +
                        reader_state.toString());
            }
            
        } catch (RETCODE_NO_DATA noData) {
            // No data to process
        } finally {
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

        // A Subscriber allows an application to create one or more DataReaders
        Subscriber subscriber =
                Objects.requireNonNull(participant.create_subscriber(
                        DomainParticipant.SUBSCRIBER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));
 
        /* If you want to change the DataWriter's QoS programmatically 
         * rather than using the XML file, you will need to add the 
         * following lines to your code and comment out the 
         * create_subscriber call above.
         */
        //Start changes for Coherent_Presentation

        // Get default subscriber QoS to customize
        /*
        SubscriberQos subscriber_qos = new SubscriberQos();
        participant.get_default_subscriber_qos(subscriber_qos);

        subscriber_qos.presentation.access_scope =
            PresentationQosPolicyAccessScopeKind.TOPIC_PRESENTATION_QOS;
        subscriber_qos.presentation.coherent_access = true;

        Subscriber subscriber = participant.create_subscriber(
            subscriber_qos, null,
            StatusKind.STATUS_MASK_NONE);
        */
      
        // End changes for Coherent_Presentation
            
        // Register the datatype to use when creating the Topic
        String typeName = coherentTypeSupport.get_type_name(); 
        coherentTypeSupport.register_type(participant, typeName);

        // Create a Topic with a name and a datatype
        Topic topic = Objects.requireNonNull(participant.create_topic(
                "Example coherent",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));

        // This DataReader reads data on "Example coherent" Topic
        reader = (coherentDataReader) Objects.requireNonNull(
                subscriber.create_datareader(
                        topic,
                        Subscriber.DATAREADER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        /* If you want to change the DataWriter's QoS programmatically 
         * rather than using the XML file, you will need to add the 
         * following lines to your code and comment out the 
         * create_datareader call above.
         */
            
        // Start changes for Coherent_Presentation

        // Get default datareader QoS to customize
        /*
            DataReaderQos datareader_qos = new DataReaderQos();
            subscriber.get_default_datareader_qos(datareader_qos);

            datareader_qos.reliability.kind =
                ReliabilityQosPolicyKind.RELIABLE_RELIABILITY_QOS;
            datareader_qos.history.depth = 10;

            reader = (coherentDataReader)
                subscriber.create_datareader(
                    topic, datareader_qos, listener,
                    StatusKind.STATUS_MASK_ALL);
        
        */
        // End changes for Coherent_Presentation
            
        // --- Wait for data --- //

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
                System.out.printf(
                        "No data after %d seconds.%n",
                        waitTimeout.sec);
            }
        }
    }

    private static class CoherentState {
        static final int nvals = 6;
        static int statevals[] = new int[nvals];

        public void set_state(char c, int value) {
            int idx = c - (int)('a');
            if (idx < 0 || idx >= nvals) {
                System.out.print("error: invalid field '" + c + "'\n");
                return;
            }
            statevals[idx] = value;
        }

        public String toString() {
            char c = 'a';
            String res = new String();
            for (int i=0; i < nvals; ++i) {
                res = res + " " + (c++) + " = " + statevals[i] + ";";
            }
            return res;
        }
    };
    
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
        try (coherentSubscriber subscriberApplication = new coherentSubscriber()) {
            subscriberApplication.parseArguments(args);
            subscriberApplication.addShutdownHook();
            subscriberApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}


        