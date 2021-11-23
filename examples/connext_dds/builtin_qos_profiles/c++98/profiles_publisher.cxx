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
    DDS_Duration_t send_period = { 4, 0 };

    /*
     * This example uses a built-in QoS profile to enable monitoring on the
     * DomainParticipant.  The code below loads the XML QoS from the
     * USER_QOS_PROFILES.xml file, which is inheriting from the
     * "BuiltinQoSLib::Generic.Monitoring.Common" profile to enable monitoring.
     *
     * !!!! NOTE: This example will only work when dynamically linking !!!
     * In Visual Studio, change the target to "Debug DLL" or "Release DLL"
     * to build.
     */
    DDSDomainParticipant *participant =
            DDSTheParticipantFactory->create_participant(
                    domain_id,
                    DDS_PARTICIPANT_QOS_DEFAULT,
                    NULL /* listener */,
                    DDS_STATUS_MASK_NONE);

    /* If you want to change the DomainParticipant's QoS programatically rather
     * than using the XML file, you will need to add the following lines to
     * your code and comment out the create_participant call above.
     *
     * This example uses a built-in QoS profile to enable
     * monitoring on the DomainParticipant.*/
    /* DDSDomainParticipant *participant = DDSTheParticipantFactory
            ->create_participant_with_profile(
                domainId, "BuiltinQosLib", "Generic.Monitoring.Common",
                NULL /* listener * /, DDS_STATUS_MASK_NONE);
     */
    if (participant == NULL) {
        return shutdown_participant(
                participant,
                "create_participant error",
                EXIT_FAILURE);
    }

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
    const char *type_name = profilesTypeSupport::get_type_name();
    DDS_ReturnCode_t retcode =
            profilesTypeSupport::register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "register_type error",
                EXIT_FAILURE);
    }

    // Create a Topic with a name and a datatype
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

    // This DataWriter writes data on "Example profiles" Topic
    DDSDataWriter *untyped_writer = publisher->create_datawriter(
            topic,
            DDS_DATAWRITER_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    /* If you want to change the DataWriter's QoS programatically rather
     * than using the XML file, you will need to add the following lines to
     * your code and comment out the create_datawriter call above.
     *
     * This example uses a built-in QoS profile to tune QoS for
     * reliable streaming data.*/
    /* To customize data writer QoS, use
       the configuration file USER_QOS_PROFILES.xml */
    /* DDSDataWriter *untyped_writer = publisher
            ->create_datawriter_with_profile(
                topic,
                DDS_BUILTIN_QOS_LIB_EXP,
                DDS_PROFILE_PATTERN_RELIABLE_STREAMING,
                NULL /* listener * /,
                DDS_STATUS_MASK_NONE);
    */
    if (untyped_writer == NULL) {
        return shutdown_participant(
                participant,
                "create_datawriter error",
                EXIT_FAILURE);
    }

    profilesDataWriter *typed_writer =
            profilesDataWriter::narrow(untyped_writer);
    if (untyped_writer == NULL) {
        return shutdown_participant(
                participant,
                "create_datawriter error",
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

    data->msg = DDS_String_dup("Hello World!");

    DDS_InstanceHandle_t instance_handle = DDS_HANDLE_NIL;
    /* For a data type that has a key, if the same instance is going to be
       written multiple times, initialize the key here
       and register the keyed instance prior to writing */
    /*
        instance_handle = profiles_writer->register_instance(*data);
    */

    // Main loop, write data
    for (unsigned int samples_written = 0;
         !shutdown_requested && samples_written < sample_count;
         ++samples_written) {
        std::cout << "Writing profiles, count " << samples_written << std::endl;

        // Modify the data to be written here
        retcode = typed_writer->write(*data, instance_handle);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "write error " << retcode << std::endl;
        }

        NDDSUtility::sleep(send_period);
    }

    /*
        retcode = typed_writer->unregister_instance(
            *data, instance_handle);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "unregister instance error " << retcode << std::endl;
        }
    */

    // Delete previously allocated profiles, including all contained elements
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
    parse_arguments(arguments, argc, argv);
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
