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
#include <stdio.h>
#include <stdlib.h>

#include "profiles.h"
#include "profilesSupport.h"
#include "ndds/ndds_cpp.h"
#include "application.h"

using namespace application;

static int shutdown_participant(
        DDSDomainParticipant *participant,
        const char *shutdown_message,
        int status);

int run_publisher_application(unsigned int domain_id, unsigned int sample_count)
{
    /* Send a new sample every second */
    DDS_Duration_t send_period = { 1, 0 };

    /* There are several different approaches for loading QoS profiles from XML
     * files (see Configuring QoS with XML chapter in the RTI Connext Core
     * Libraries and Utilities User's Manual). In this example we illustrate
     * two of them:
     *
     * 1) Creating a file named USER_QOS_PROFILES.xml, which is loaded,
     * automatically by the DomainParticipantFactory. In this case, the file
     * defines a QoS profile named volatile_profile that configures reliable,
     * volatile DataWriters and DataReaders.
     *
     * 2) Adding XML documents to the DomainParticipantFactory using its
     * Profile QoSPolicy (DDS Extension). In this case, we add
     * my_custom_qos_profiles.xml to the url_profile sequence, which stores
     * the URLs of all the XML documents with QoS policies that are loaded by
     * the DomainParticipantFactory aside from the ones that are automatically
     * loaded.
     * my_custom_qos_profiles.xml defines a QoS profile named
     * transient_local_profile that configures reliable, transient local
     * DataWriters and DataReaders.
     */

    /* To load my_custom_qos_profiles.xml, as explained above, we need to modify
     * the  DDSTheParticipantFactory Profile QoSPolicy */
    DDS_DomainParticipantFactoryQos factory_qos;
    DDSTheParticipantFactory->get_qos(factory_qos);

    /* We are only going to add one XML file to the url_profile sequence, so we
     * ensure a length of 1,1. */
    factory_qos.profile.url_profile.ensure_length(1, 1);

    /* The XML file will be loaded from the working directory. That means, you
     * need to run the example like this:
     * ./objs/<architecture>/profiles_publisher
     * (see README.txt for more information on how to run the example).
     *
     * Note that you can specify the absolute path of the XML QoS file to avoid
     * this problem.
     */
    factory_qos.profile.url_profile[0] =
            DDS_String_dup("my_custom_qos_profiles.xml");

    DDSTheParticipantFactory->set_qos(factory_qos);


    /* Our default Qos profile, volatile_profile, sets the participant name.
     * This is the only participant_qos policy that we change in our
     * example. As this is done in the default QoS profile, we don't need
     * to specify its name, so we can create the participant using the
     * create_participant() method rather than using
     * create_participant_with_profile().  */
    DDSDomainParticipant *participant =
            DDSTheParticipantFactory->create_participant(
                    domain_id,
                    DDS_PARTICIPANT_QOS_DEFAULT,
                    NULL /* listener */,
                    DDS_STATUS_MASK_NONE);
    if (participant == NULL) {
        return shutdown_participant(
                participant,
                "create_participant error",
                EXIT_FAILURE);
    }

    /* We haven't changed the publisher_qos in any of QoS profiles we use in
     * this example, so we can just use the create_publisher() method. If you
     * want to load an specific profile in which you may have changed the
     * publisher_qos, use the create_publisher_with_profile() method. */
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
    const char *type_name = profilesTypeSupport::get_type_name();
    DDS_ReturnCode_t retcode =
            profilesTypeSupport::register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "register_type error",
                EXIT_FAILURE);
    }

    /* We haven't changed the topic_qos in any of QoS profiles we use in this
     * example, so we can just use the create_topic() method. If you want to
     * load an specific profile in which you may have changed the topic_qos,
     * use the create_topic_with_profile() method. */
    DDSTopic *topic = participant->create_topic(
            "Example profiles",
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

    /* Volatile writer -- As volatile_profile is the default qos profile
     * we don't need to specify the profile we are going to use, we can
     * just call create_datawriter passing DDS_DATAWRITER_QOS_DEFAULT. */
    DDSDataWriter *writer_volatile = publisher->create_datawriter(
            topic,
            DDS_DATAWRITER_QOS_DEFAULT, /* Default datawriter_qos */
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (writer_volatile == NULL) {
        return shutdown_participant(
                participant,
                "create_datawriter error",
                EXIT_FAILURE);
    }

    /* Transient Local writer -- In this case we use
     * create_datawriter_with_profile, because we have to use a profile other
     * than the default one. This profile has been defined in
     * my_custom_qos_profiles.xml, but since we already loaded the XML file
     * we don't need to specify anything else. */
    DDSDataWriter *writer_transient_local =
            publisher->create_datawriter_with_profile(
                    topic,                     /* DDS_Topic* */
                    "profiles_Library",        /* library_name */
                    "transient_local_profile", /* profile_name */
                    NULL /* listener */,
                    DDS_STATUS_MASK_NONE /* DDS_StatusMask */
            );
    if (writer_transient_local == NULL) {
        return shutdown_participant(
                participant,
                "create_datawriter error",
                EXIT_FAILURE);
    }

    profilesDataWriter *typed_writer_volatile =
            profilesDataWriter::narrow(writer_volatile);
    if (typed_writer_volatile == NULL) {
        return shutdown_participant(
                participant,
                "DataWriter narrow error",
                EXIT_FAILURE);
    }

    profilesDataWriter *typed_writer_transient_local =
            profilesDataWriter::narrow(writer_transient_local);
    if (typed_writer_transient_local == NULL) {
        return shutdown_participant(
                participant,
                "DataWriter narrow error",
                EXIT_FAILURE);
    }

    // Create data for writing, allocating all members
    profiles *data = profilesTypeSupport::create_data();
    if (data == NULL) {
        return shutdown_participant(
                participant,
                "profilesTypeSupport::create_data error",
                EXIT_FAILURE);
    }

    // Main loop, write data
    for (unsigned int samples_written = 0;
         !shutdown_requested && samples_written < sample_count;
         ++samples_written) {
        // Modify the data to be sent here
        strcpy(data->profile_name, "volatile_profile");
        data->x = samples_written;

        std::cout << "Writing profile_name = " << data->profile_name
                  << ",\t x = " << data->x << std::endl;

        retcode = typed_writer_volatile->write(*data, DDS_HANDLE_NIL);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "write error " << retcode << std::endl;
        }

        strcpy(data->profile_name, "transient_local_profile");
        data->x = samples_written;

        std::cout << "Writing profile_name = " << data->profile_name
                  << ",\t x = " << data->x << std::endl;

        retcode = typed_writer_transient_local->write(*data, DDS_HANDLE_NIL);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "write error " << retcode << std::endl;
        }


        NDDSUtility::sleep(send_period);
    }

    /* Delete data sample */
    retcode = profilesTypeSupport::delete_data(data);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "profilesTypeSupport::delete_data error " << retcode
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
