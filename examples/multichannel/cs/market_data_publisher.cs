using System;
using System.Collections.Generic;
using System.Text;
/* market_data_publisher.cs

   A publication of data of type market_data

   This file is derived from code automatically generated by the rtiddsgen 
   command:

   rtiddsgen -language C# -example <arch> market_data.idl

   Example publication of type market_data automatically generated by 
   'rtiddsgen'. To test them follow these steps:

   (1) Compile this file and the example subscription.

   (2) Start the subscription with the command
       objs\<arch>\market_data_subscriber <domain_id> <sample_count>
                
   (3) Start the publication with the command
       objs\<arch>\market_data_publisher <domain_id> <sample_count>

   (4) [Optional] Specify the list of discovery initial peers and 
       multicast receive addresses via an environment variable or a file 
       (in the current working directory) called NDDS_DISCOVERY_PEERS. 

   You can run any number of publishers and subscribers programs, and can 
   add and remove them dynamically from the domain.


   Example:

       To run the example application on domain <domain_id>:

       bin\<Debug|Release>\market_data_publisher <domain_id> <sample_count>
       bin\<Debug|Release>\market_data_subscriber <domain_id> <sample_count>

       
modification history
------------ -------       
*/

public class market_dataPublisher {

    public static void Main( string[] args ) {

        // --- Get domain ID --- //
        int domain_id = 0;
        if (args.Length >= 1) {
            domain_id = Int32.Parse(args[0]);
        }

        // --- Get max loop count; 0 means infinite loop  --- //
        int sample_count = 0;
        if (args.Length >= 2) {
            sample_count = Int32.Parse(args[1]);
        }

        /* Uncomment this to turn on additional logging
        NDDS.ConfigLogger.get_instance().set_verbosity_by_category(
            NDDS.LogCategory.NDDS_CONFIG_LOG_CATEGORY_API, 
            NDDS.LogVerbosity.NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL);
        */

        // --- Run --- //
        try {
            market_dataPublisher.publish(
                domain_id, sample_count);
        } catch (DDS.Exception) {
            Console.WriteLine("error in publisher");
        }
    }

    static void publish( int domain_id, int sample_count ) {

        // --- Create participant --- //

        /* To customize participant QoS, use 
           the configuration file USER_QOS_PROFILES.xml */
        DDS.DomainParticipant participant =
            DDS.DomainParticipantFactory.get_instance().create_participant(
                domain_id,
                DDS.DomainParticipantFactory.PARTICIPANT_QOS_DEFAULT,
                null /* listener */,
                DDS.StatusMask.STATUS_MASK_NONE);
        if (participant == null) {
            shutdown(participant);
            throw new ApplicationException("create_participant error");
        }

        // --- Create publisher --- //

        /* To customize publisher QoS, use 
           the configuration file USER_QOS_PROFILES.xml */
        DDS.Publisher publisher = participant.create_publisher(
        DDS.DomainParticipant.PUBLISHER_QOS_DEFAULT,
        null /* listener */,
        DDS.StatusMask.STATUS_MASK_NONE);
        if (publisher == null) {
            shutdown(participant);
            throw new ApplicationException("create_publisher error");
        }

        // --- Create topic --- //

        /* Register type before creating topic */
        System.String type_name = market_dataTypeSupport.get_type_name();
        try {
            market_dataTypeSupport.register_type(
                participant, type_name);
        } catch (DDS.Exception e) {
            Console.WriteLine("register_type error {0}", e);
            shutdown(participant);
            throw e;
        }

        /* To customize topic QoS, use 
           the configuration file USER_QOS_PROFILES.xml */
        DDS.Topic topic = participant.create_topic(
            "Example market_data",
            type_name,
            DDS.DomainParticipant.TOPIC_QOS_DEFAULT,
            null /* listener */,
            DDS.StatusMask.STATUS_MASK_NONE);
        if (topic == null) {
            shutdown(participant);
            throw new ApplicationException("create_topic error");
        }

        /* If you want to change the DataWriter's QoS programmatically rather 
         * than using the XML file, you will need to add the following lines to 
         * your code and modify the datawriter creation fuction using writer_qos.
         *
         * In this case, we set the publish as multichannel using the differents
         * channel to send differents symbol. Every channel have a IP to send 
         * the data.
         */

        /* Start changes for MultiChannel */
        /*
        DDS.DataWriterQos writer_qos = new DDS.DataWriterQos();
        try {
            publisher.get_default_datawriter_qos(writer_qos);
        } catch (DDS.Exception e) {
            Console.WriteLine("get_default_datawriter_qos error {0}", e);
            shutdown(participant);
            throw e;
        }
        */
        /* Create 8 channels based on Symbol */
        /*
        writer_qos.multi_channel.channels.ensure_length(8, 8);
        System.Int32 length = writer_qos.multi_channel.channels.length;
        for (int i = 0; i < length; i++) {
            DDS.ChannelSettings_t channelSetting =
                new DDS.ChannelSettings_t();
            writer_qos.multi_channel.channels.set_at(i, channelSetting);
            DDS.TransportMulticastSettingsSeq multiCastSettingSeq =
                new DDS.TransportMulticastSettingsSeq(1);
            writer_qos.multi_channel.channels.get_at(i).
                multicast_settings = multiCastSettingSeq;
            writer_qos.multi_channel.channels.get_at(i).
                multicast_settings.ensure_length(1, 1);
            DDS.TransportMulticastSettings_t multicastSetting =
                new DDS.TransportMulticastSettings_t();
            writer_qos.multi_channel.channels.get_at(i).
                multicast_settings.set_at(0, multicastSetting);
        }
        writer_qos.multi_channel.channels.get_at(0).
            filter_expression = "Symbol MATCH '[A-C]*'";
        writer_qos.multi_channel.channels.get_at(0).
            multicast_settings.ensure_length(1, 1);
        writer_qos.multi_channel.channels.get_at(0).
            multicast_settings.get_at(0).receive_address = "239.255.0.2";
        writer_qos.multi_channel.channels.get_at(1).
            filter_expression = "Symbol MATCH '[D-F]*'";
        writer_qos.multi_channel.channels.get_at(1).
            multicast_settings.ensure_length(1, 1);
        writer_qos.multi_channel.channels.get_at(1).
            multicast_settings.get_at(0).receive_address = "239.255.0.3";
        writer_qos.multi_channel.channels.get_at(2).
            filter_expression = "Symbol MATCH '[G-I]*'";
        writer_qos.multi_channel.channels.get_at(2).
            multicast_settings.ensure_length(1, 1);
        writer_qos.multi_channel.channels.get_at(2).
            multicast_settings.get_at(0).receive_address = "239.255.0.4";
        writer_qos.multi_channel.channels.get_at(3).
            filter_expression = "Symbol MATCH '[J-L]*'";
        writer_qos.multi_channel.channels.get_at(3).
            multicast_settings.ensure_length(1, 1);
        writer_qos.multi_channel.channels.get_at(3).
            multicast_settings.get_at(0).receive_address = "239.255.0.5";
        writer_qos.multi_channel.channels.get_at(4).
            filter_expression = "Symbol MATCH '[M-O]*'";
        writer_qos.multi_channel.channels.get_at(4).
            multicast_settings.ensure_length(1, 1);
        writer_qos.multi_channel.channels.get_at(4).
            multicast_settings.get_at(0).receive_address = "239.255.0.6";
        writer_qos.multi_channel.channels.get_at(5).
            filter_expression = "Symbol MATCH '[P-S]*'";
        writer_qos.multi_channel.channels.get_at(5).
            multicast_settings.ensure_length(1, 1);
        writer_qos.multi_channel.channels.get_at(5).
            multicast_settings.get_at(0).receive_address = "239.255.0.7";
        writer_qos.multi_channel.channels.get_at(6).
            filter_expression = "Symbol MATCH '[T-V]*'";
        writer_qos.multi_channel.channels.get_at(6).
            multicast_settings.ensure_length(1, 1);
        writer_qos.multi_channel.channels.get_at(6).
            multicast_settings.get_at(0).receive_address = "239.255.0.8";
        writer_qos.multi_channel.channels.get_at(7).
            filter_expression = "Symbol MATCH '[W-Z]*'";
        writer_qos.multi_channel.channels.get_at(7).
            multicast_settings.ensure_length(1, 1);
        writer_qos.multi_channel.channels.get_at(7).
            multicast_settings.get_at(0).receive_address = "239.255.0.9";
        */
        // --- Create writer --- //

        /* To customize data writer QoS, use 
           the configuration file USER_QOS_PROFILES.xml */
        /* toggle between writer_qos and DDS_DATAWRITER_QOS_DEFAULT to alternate
         * between using code and using XML to specify the Qos */
        DDS.DataWriter writer = publisher.create_datawriter(
            topic,
            /* writer_qos */DDS.Publisher.DATAWRITER_QOS_DEFAULT,
            null /* listener */,
            DDS.StatusMask.STATUS_MASK_NONE);
        if (writer == null) {
            shutdown(participant);
            throw new ApplicationException("create_datawriter error");
        }

        market_dataDataWriter market_data_writer =
            (market_dataDataWriter)writer;

        /* End changes for MultiChannel */

        // --- Write --- //

        /* Create data sample for writing */
        market_data instance = market_dataTypeSupport.create_data();
        if (instance == null) {
            shutdown(participant);
            throw new ApplicationException(
                "market_dataTypeSupport.create_data error");
        }

        /* For a data type that has a key, if the same instance is going to be
           written multiple times, initialize the key here
           and register the keyed instance prior to writing */
        DDS.InstanceHandle_t instance_handle = DDS.InstanceHandle_t.HANDLE_NIL;
        /*
        instance_handle = market_data_writer.register_instance(instance);
        */

        /* Changes for MultiChannel */
        const System.Int32 send_period = 100; // milliseconds

        /* Main loop */
        for (int count = 0;
             (sample_count == 0) || (count < sample_count);
             ++count) {
            /* Console.WriteLine("Writing market_data, count {0}", count); */

            /* Changes for MultiChannel */
            /* Modify the data to be sent here */
            char[] SymbolBuffer = { (char)('A' + (char)(count % 26)) };
            String Symbol = new String(SymbolBuffer);
            instance.Symbol = Symbol;
            instance.Price = count;


            try {
                market_data_writer.write(instance, ref instance_handle);
            } catch (DDS.Exception e) {
                Console.WriteLine("write error {0}", e);
            }

            System.Threading.Thread.Sleep(send_period);
        }

        /*
        try {
            market_data_writer.unregister_instance(
                instance, ref instance_handle);
        } catch(DDS.Exception e) {
            Console.WriteLine("unregister instance error: {0}", e);
        }
        */

        // --- Shutdown --- //

        /* Delete data sample */
        try {
            market_dataTypeSupport.delete_data(instance);
        } catch (DDS.Exception e) {
            Console.WriteLine(
                "market_dataTypeSupport.delete_data error: {0}", e);
        }

        /* Delete all entities */
        shutdown(participant);
    }

    static void shutdown(
        DDS.DomainParticipant participant ) {

        /* Delete all entities */

        if (participant != null) {
            participant.delete_contained_entities();
            DDS.DomainParticipantFactory.get_instance().delete_participant(
                ref participant);
        }

        /* RTI Connext provides finalize_instance() method on
           domain participant factory for people who want to release memory
           used by the participant factory. Uncomment the following block of
           code for clean destruction of the singleton. */
        /*
        try {
            DDS.DomainParticipantFactory.finalize_instance();
        } catch (DDS.Exception e) {
            Console.WriteLine("finalize_instance error: {0}", e);
            throw e;
        }
        */
    }
}

