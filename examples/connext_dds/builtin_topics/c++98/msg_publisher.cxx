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

#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>

#include "msg.h"
#include "msgSupport.h"
#include "ndds/ndds_cpp.h"
#include "application.h"

using namespace application;

/* Authorization string. */
const char *auth = "password";

static int shutdown_participant(
        DDSDomainParticipant *participant,
        const char *shutdown_message,
        int status);

/* The builtin subscriber sets participant_qos.user_data and
   so we set up listeners for the builtin
   DataReaders to access these fields.
*/

class BuiltinParticipantListener : public DDSDataReaderListener {
public:
    virtual void on_data_available(DDSDataReader *reader);
};

/* This gets called when a participant has been discovered */
void BuiltinParticipantListener::on_data_available(DDSDataReader *reader)
{
    DDSParticipantBuiltinTopicDataDataReader *builtin_reader =
            (DDSParticipantBuiltinTopicDataDataReader *) reader;
    DDS_ParticipantBuiltinTopicDataSeq data_seq;
    DDS_SampleInfoSeq info_seq;
    DDS_ReturnCode_t retcode;

    const char *participant_data;

    /* We only process newly seen participants */
    retcode = builtin_reader->take(
            data_seq,
            info_seq,
            DDS_LENGTH_UNLIMITED,
            DDS_ANY_SAMPLE_STATE,
            DDS_NEW_VIEW_STATE,
            DDS_ANY_INSTANCE_STATE);

    /* This happens when we get announcements from participants we
     * already know about
     */
    if (retcode == DDS_RETCODE_NO_DATA)
        return;

    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "Error: failed to access data from the built-in reader\n";
        return;
    }

    for (int i = 0; i < data_seq.length(); ++i) {
        if (!info_seq[i].valid_data)
            continue;

        participant_data = "nil";
        bool is_auth = false;
        /* see if there is any participant_data */
        if (data_seq[i].user_data.value.length() != 0) {
            /* This sequence is guaranteed to be contiguous */
            participant_data = (char *) &data_seq[i].user_data.value[0];
            is_auth = (strcmp(participant_data, auth) == 0);
        }

        std::ios::fmtflags default_format(std::cout.flags());
        std::cout << std::hex << std::setw(8) << std::setfill('0');

        std::cout << "Built-in Reader: found participant \n";
        std::cout << "\tkey->'" << data_seq[i].key.value[0] << " "
                  << data_seq[i].key.value[1] << " " << data_seq[i].key.value[2]
                  << "'\n";
        std::cout << "\tuser_data->'" << participant_data << "'\n";

        int ih[6];
        memcpy(ih,
               &info_seq[i].instance_handle,
               sizeof(info_seq[i].instance_handle));
        std::cout << "instance_handle: " << ih[0] << ih[1] << " " << ih[2]
                  << ih[3] << " " << ih[4] << ih[5] << std::endl;

        std::cout.flags(default_format);

        if (!is_auth) {
            std::cout << "Bad authorization, ignoring participant\n";
            DDSDomainParticipant *participant =
                    reader->get_subscriber()->get_participant();

            retcode = participant->ignore_participant(
                    info_seq[i].instance_handle);
            if (retcode != DDS_RETCODE_OK) {
                std::cout << "error ignoring participant: " << retcode
                          << std::endl;
                return;
            }
        }
    }

    builtin_reader->return_loan(data_seq, info_seq);
}

class BuiltinSubscriberListener : public DDSDataReaderListener {
public:
    virtual void on_data_available(DDSDataReader *reader);
};

/* This gets called when a new subscriber has been discovered */
void BuiltinSubscriberListener::on_data_available(DDSDataReader *reader)
{
    DDSSubscriptionBuiltinTopicDataDataReader *builtin_reader =
            (DDSSubscriptionBuiltinTopicDataDataReader *) reader;
    DDS_SubscriptionBuiltinTopicDataSeq data_seq;
    DDS_SampleInfoSeq info_seq;
    DDS_ReturnCode_t retcode;

    /* We only process newly seen subscribers */
    retcode = builtin_reader->take(
            data_seq,
            info_seq,
            DDS_LENGTH_UNLIMITED,
            DDS_ANY_SAMPLE_STATE,
            DDS_NEW_VIEW_STATE,
            DDS_ANY_INSTANCE_STATE);

    if (retcode == DDS_RETCODE_NO_DATA)
        return;

    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "Error: failed to access data from the built-in reader\n";
        return;
    }

    for (int i = 0; i < data_seq.length(); ++i) {
        if (!info_seq[i].valid_data)
            continue;

        std::ios::fmtflags default_format(std::cout.flags());
        std::cout << std::hex << std::setw(8) << std::setfill('0');

        std::cout << "Built-in Reader: found subscriber \n";
        std::cout << "\tparticipant_key->'"
                  << data_seq[i].participant_key.value[0] << " "
                  << data_seq[i].participant_key.value[1] << " "
                  << data_seq[i].participant_key.value[2] << "'\n";
        std::cout << "\tkey->'" << data_seq[i].key.value[0] << " "
                  << data_seq[i].key.value[1] << " " << data_seq[i].key.value[2]
                  << "'\n";

        int ih[6];
        memcpy(ih,
               &info_seq[i].instance_handle,
               sizeof(info_seq[i].instance_handle));
        std::cout << "instance_handle: " << ih[0] << ih[1] << " " << ih[2]
                  << ih[3] << " " << ih[4] << ih[5] << std::endl;

        std::cout.flags(default_format);
    }

    builtin_reader->return_loan(data_seq, info_seq);
}

int run_publisher_application(unsigned int domain_id, unsigned int sample_count)
{
    DDS_Duration_t send_period = { 1, 0 };

    /* By default, the participant is enabled upon construction.
     * At that time our listeners for the builtin topics have not
     * been installed, so we disable the participant until we
     * set up the listeners. This is done by default in the
     * USER_QOS_PROFILES.xml
     * file. If you want to do it programmatically, just uncomment
     * the following code.
     */
    /*
    DDS_DomainParticipantFactoryQos factory_qos;
    DDS_ReturnCode_t retcode = DDSTheParticipantFactory->get_qos(factory_qos);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(participant, "create_participant error",
    EXIT_FAILURE);
    }

    factory_qos.entity_factory.autoenable_created_entities = DDS_BOOLEAN_FALSE;

    switch(DDSTheParticipantFactory->set_qos(factory_qos)) {
        case DDS_RETCODE_OK:
            break;
        case DDS_RETCODE_IMMUTABLE_POLICY: {
            std::cerr << "Cannot set factory Qos due to IMMUTABLE_POLICY ";
            std::cerr << "for domain participant\n";
            return EXIT_FAILURE;
            break;
        }
        case DDS_RETCODE_INCONSISTENT_POLICY: {
            std::cerr << "Cannot set factory Qos due to INCONSISTENT_POLICY for ";
            std::cerr << "domain participant\n";
            return EXIT_FAILURE;
            break;
        }
        default: {
            std::cerr << "Cannot set factory Qos for unknown reason for ";
            std::cerr <<"domain participant\n";
            return EXIT_FAILURE;
            break;
        }
    }
    */

    DDS_DomainParticipantQos participant_qos;
    DDS_ReturnCode_t retcode =
            DDSTheParticipantFactory->get_default_participant_qos(
                    participant_qos);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "get_default_participant_qos error" << std::endl;
        return EXIT_FAILURE;
    }

    /* If you want to change the Participant's QoS programmatically rather
     * than using the XML file, you will need to uncomment the following line.
     */
    /*
    participant_qos.resource_limits.participant_user_data_max_length = 1024;
    */

    // Start communicating in a domain, usually one participant per application
    DDSDomainParticipant *participant =
            DDSTheParticipantFactory->create_participant(
                    domain_id,
                    participant_qos,
                    NULL /* listener */,
                    DDS_STATUS_MASK_NONE);
    if (participant == NULL) {
        return shutdown_participant(
                participant,
                "create_participant error",
                EXIT_FAILURE);
    }

    // Start changes for Builtin_Topics
    // Installing listeners for the builtin topics requires several steps

    // A Subscriber allows an application to create one or more DataReaders
    DDSSubscriber *builtin_subscriber = participant->get_builtin_subscriber();
    if (builtin_subscriber == NULL) {
        return shutdown_participant(
                participant,
                "failed to create builtin subscriber",
                EXIT_FAILURE);
    }

    /* Then get builtin subscriber's datareader for participants
       The type name is a bit hairy, but can be read right to left:
       DDSParticipantBuiltinTopicDataDataReader is a DataReader for
       BuiltinTopicData concerning a discovered
       Participant
    */
    DDSParticipantBuiltinTopicDataDataReader *builtin_participant_datareader =
            (DDSParticipantBuiltinTopicDataDataReader *) builtin_subscriber
                    ->lookup_datareader(DDS_PARTICIPANT_TOPIC_NAME);
    if (builtin_participant_datareader == NULL) {
        return shutdown_participant(
                participant,
                "failed to create builtin participant data reader",
                EXIT_FAILURE);
    }

    // Install our listener
    BuiltinParticipantListener *builtin_participant_listener =
            new BuiltinParticipantListener();
    builtin_participant_datareader->set_listener(
            builtin_participant_listener,
            DDS_DATA_AVAILABLE_STATUS);

    // Get builtin subscriber's datareader for subscribers
    DDSSubscriptionBuiltinTopicDataDataReader *builtin_subscription_datareader =
            (DDSSubscriptionBuiltinTopicDataDataReader *) builtin_subscriber
                    ->lookup_datareader(DDS_SUBSCRIPTION_TOPIC_NAME);
    if (builtin_subscription_datareader == NULL) {
        return shutdown_participant(
                participant,
                "failed to create builtin subscription data reader",
                EXIT_FAILURE);
    }

    // Install our listener
    BuiltinSubscriberListener *builtin_subscriber_listener =
            new BuiltinSubscriberListener();
    builtin_subscription_datareader->set_listener(
            builtin_subscriber_listener,
            DDS_DATA_AVAILABLE_STATUS);

    /* Done!  All the listeners are installed, so we can enable the
     * participant now.
     */
    if (participant->enable() != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "failed to Enable Participant",
                EXIT_FAILURE);
    }
    /* End changes for Builtin_Topics */

    // A Publisher allows an application to create one or more DataWriters
    DDSPublisher *publisher = participant->create_publisher(
            DDS_PUBLISHER_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (publisher == NULL) {
        return shutdown_participant(
                participant,
                "create_publisher error",
                EXIT_FAILURE);
    }

    // Register the datatype to use when creating the Topic
    const char *type_name = msgTypeSupport::get_type_name();
    retcode = msgTypeSupport::register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "register_type error",
                EXIT_FAILURE);
    }

    // Create a Topic with a name and a datatype
    DDSTopic *topic = participant->create_topic(
            "Example msg",
            type_name,
            DDS_TOPIC_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (topic == NULL) {
        return shutdown_participant(
                participant,
                "create_topic error",
                EXIT_FAILURE);
    }

    /* To customize data writer QoS, use
       the configuration file USER_QOS_PROFILES.xml */
    DDSDataWriter *untyped_writer = publisher->create_datawriter(
            topic,
            DDS_DATAWRITER_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (untyped_writer == NULL) {
        return shutdown_participant(
                participant,
                "create_datawriter error",
                EXIT_FAILURE);
    }

    msgDataWriter *typed_writer = msgDataWriter::narrow(untyped_writer);
    if (typed_writer == NULL) {
        return shutdown_participant(
                participant,
                "DataWriter narrow error",
                EXIT_FAILURE);
    }

    /* Create data sample for writing */
    msg *data = msgTypeSupport::create_data();
    if (data == NULL) {
        return shutdown_participant(
                participant,
                "msgTypeSupport::create_data error",
                EXIT_FAILURE);
    }

    /* For data type that has key, if the same instance is going to be
       written multiple times, initialize the key here
       and register the keyed instance prior to writing */
    /*
    DDS_InstanceHandle_t instance_handle = typed_writer
            ->register_instance(*data);
    */

    // Main loop, write data
    for (unsigned int samples_written = 0;
         !shutdown_requested && samples_written < sample_count;
         ++samples_written) {
        NDDSUtility::sleep(send_period);

        std::cout << "Writing msg, count " << samples_written << std::endl;

        // Modify the data to be sent here
        data->x = samples_written;

        retcode = typed_writer->write(*data, DDS_HANDLE_NIL);
        if (retcode != DDS_RETCODE_OK) {
            printf("write error %d\n", retcode);
        }
    }

    /*
    retcode = typed_writer->unregister_instance(
      *data, DDS_HANDLE_NIL);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "unregister instance error " << retcode << std::endl;
    }
    */

    // Delete previously allocated async, including all contained elements
    retcode = msgTypeSupport::delete_data(data);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "msgTypeSupport::delete_data error " << retcode
                  << std::endl;
    }

    // Delete all entities (DataWriter, Topic, Publisher, DomainParticipant)
    return shutdown_participant(participant, "Shutting down", EXIT_SUCCESS);
}

// Delete all entities
static int shutdown_participant(
        DDSDomainParticipant *participant,
        const char *shutdown_message,
        int status)
{
    DDS_ReturnCode_t retcode;

    std::cout << shutdown_message << std::endl;

    if (participant != NULL) {
        // Cleanup everything created by this Participant
        retcode = participant->delete_contained_entities();
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "delete_contained_entities error " << retcode
                      << std::endl;
            status = EXIT_FAILURE;
        }

        retcode = DDSTheParticipantFactory->delete_participant(participant);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "delete_participant error " << retcode << std::endl;
            status = EXIT_FAILURE;
        }
    }

    return status;
}

int main(int argc, char *argv[])
{
    // Parse arguments and handle control-C
    ApplicationArguments arguments;
    parse_arguments(arguments, argc, argv, publisher);
    if (arguments.parse_result == PARSE_RETURN_EXIT) {
        return EXIT_SUCCESS;
    } else if (arguments.parse_result == PARSE_RETURN_FAILURE) {
        return EXIT_FAILURE;
    }
    setup_signal_handlers();

    // Sets Connext verbosity to help debugging
    NDDSConfigLogger::get_instance()->set_verbosity(arguments.verbosity);

    int status = run_publisher_application(
            arguments.domain_id,
            arguments.sample_count);

    // Releases the memory used by the participant factory.  Optional at
    // application exit
    DDS_ReturnCode_t retcode = DDSDomainParticipantFactory::finalize_instance();
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "finalize_instance error " << retcode << std::endl;
        status = EXIT_FAILURE;
    }

    return status;
}
