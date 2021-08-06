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

import java.net.InetAddress;
import java.util.Objects;

import com.rti.dds.domain.DomainParticipant;
import com.rti.dds.domain.DomainParticipantFactory;
import com.rti.dds.infrastructure.InstanceHandle_t;
import com.rti.dds.infrastructure.StatusKind;
import com.rti.dds.publication.DataWriterQos;
import com.rti.dds.publication.Publisher;
import com.rti.dds.topic.Topic;


public class market_dataPublisher extends Application implements AutoCloseable {
    // Usually one per application
    private DomainParticipant participant = null;

    private void runApplication() {

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
        String typeName = market_dataTypeSupport.get_type_name();
        market_dataTypeSupport.register_type(participant, typeName);

        // Create a Topic with a name and a datatype
        Topic topic = Objects.requireNonNull(participant.create_topic(
                "Example market_data",
                typeName,
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null,  // listener
                StatusKind.STATUS_MASK_NONE));

        /*
         * If you want to change the DataWriter's QoS programmatically
         * rather than using the XML file, you will need to add the
         * following lines to your code and modify the datawriter creation
         * fuction using writer_qos.
         * 
         * In this case, we set the publish as multichannel using the
         * differents channel to send differents symbol. Every channel have
         * a IP to send the data.
         */
        // Start changes for MultiChannel
        /*      
        DataWriterQos writer_qos = new DataWriterQos();
        try {
            publisher.get_default_datawriter_qos(writer_qos);
        } catch (Exception e) {
            System.err.println("! Unable to get_default_datawriter_qos "
                    + e);
        }

        writer_qos.multi_channel.channels.setMaximum(8);
        ChannelSettings_t channel = new ChannelSettings_t();
        channel.multicast_settings.setMaximum(1);
        TransportMulticastSettings_t multicast_setting = 
                new TransportMulticastSettings_t();
        try {
            channel.filter_expression = "Symbol MATCH '[A-C]*'";
            multicast_setting.receive_address = InetAddress
                    .getByName("239.255.0.2");
            channel.multicast_settings.add(multicast_setting);
            writer_qos.multi_channel.channels.add(new ChannelSettings_t(
                    channel));

            channel.filter_expression = "Symbol MATCH '[D-F]*'";
            multicast_setting.receive_address = InetAddress
                    .getByName("239.255.0.3");
            channel.multicast_settings.set(0, multicast_setting);
            writer_qos.multi_channel.channels.add(new ChannelSettings_t(
                    channel));

            channel.filter_expression = "Symbol MATCH '[G-I]*'";
            multicast_setting.receive_address = InetAddress
                    .getByName("239.255.0.4");
            channel.multicast_settings.set(0, multicast_setting);
            writer_qos.multi_channel.channels.add(new ChannelSettings_t(
                    channel));

            channel.filter_expression = "Symbol MATCH '[J-L]*'";
            multicast_setting.receive_address = InetAddress
                    .getByName("239.255.0.5");
            channel.multicast_settings.set(0, multicast_setting);
            writer_qos.multi_channel.channels.add(new ChannelSettings_t(
                    channel));

            channel.filter_expression = "Symbol MATCH '[M-O]*'";
            multicast_setting.receive_address = InetAddress
                    .getByName("239.255.0.6");
            channel.multicast_settings.set(0, multicast_setting);
            writer_qos.multi_channel.channels.add(new ChannelSettings_t(
                    channel));

            channel.filter_expression = "Symbol MATCH '[P-S]*'";
            multicast_setting.receive_address = InetAddress
                    .getByName("239.255.0.7");
            channel.multicast_settings.set(0, multicast_setting);
            writer_qos.multi_channel.channels.add(new ChannelSettings_t(
                    channel));

            channel.filter_expression = "Symbol MATCH '[T-V]*'";
            multicast_setting.receive_address = InetAddress
                    .getByName("239.255.0.8");
            channel.multicast_settings.set(0, multicast_setting);
            writer_qos.multi_channel.channels.add(new ChannelSettings_t(
                    channel));

            channel.filter_expression = "Symbol MATCH '[W-Z]*'";
            multicast_setting.receive_address = InetAddress
                    .getByName("239.255.0.9");
            channel.multicast_settings.set(0, multicast_setting);
            writer_qos.multi_channel.channels.add(new ChannelSettings_t(
                    channel));
        } catch (Exception e) {
            System.err.println("Unable to create qos settings: " + e);
        }
        */

        /*
         * toggle between writer_qos and DDS_DATAWRITER_QOS_DEFAULT to
         * alternate between using code and using XML to specify the Qos
         */
        market_dataDataWriter writer = (market_dataDataWriter) Objects.requireNonNull(
                publisher.create_datawriter(
                        topic,
                        Publisher.DATAWRITER_QOS_DEFAULT,
                        null,  // listener
                        StatusKind.STATUS_MASK_NONE));

        // End changes for MultiChannel

        // Create data sample for writing
        market_data data = new market_data();

        InstanceHandle_t instance_handle = InstanceHandle_t.HANDLE_NIL;
        /*
         * For a data type that has a key, if the same instance is going to
         * be written multiple times, initialize the key here and register
         * the keyed instance prior to writing
         */
        // instance_handle = writer.register_instance(data);

        final long sendPeriodMillis = 100; // 0.1 seconds

        for (int samplesWritten = 0;
             !isShutdownRequested() && samplesWritten < getMaxSampleCount();
             samplesWritten++) {
                
            // System.out.println("Writing market_data, count " + count);

            // Changes for MultiChannel
            // Modify the instance to be written here
            char SymbolBuffer[] = { (char) ('A' + (char) (samplesWritten % 26)) };
            String Symbol = new String(SymbolBuffer);
            data.Symbol = Symbol;
            data.Price = samplesWritten;

            // Write data
            writer.write(data, instance_handle);
            try {
                Thread.sleep(sendPeriodMillis);
            } catch (InterruptedException ix) {
                System.err.println("INTERRUPTED");
                break;
            }
        }

        // writer.unregister_instance(data, instance_handle);
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
        try (market_dataPublisher publisherApplication = new market_dataPublisher()) {
            publisherApplication.parseArguments(args);
            publisherApplication.addShutdownHook();
            publisherApplication.runApplication();
        }

        // Releases the memory used by the participant factory. Optional at
        // application exit.
        DomainParticipantFactory.finalize_instance();
    }
}
