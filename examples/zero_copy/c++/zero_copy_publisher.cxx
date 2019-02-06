/*******************************************************************************
 (c) 2005-2017 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>

#include "zero_copy.h"
#include "zero_copySupport.h"
#include "FrameSupport.h"
#include "ndds/ndds_cpp.h"
#include "ndds/ndds_namespace_cpp.h"
#include "ndds/osapi/osapi_utility.h"
#include "ndds/clock/clock_highResolution.h"
#include "ndds/osapi/osapi_ntptime.h"

using namespace DDS;

struct CommandLineArguments {
    int domain_id;
    int frame_count;
    int frame_rate;
    int sample_count;
    int key;
    int size;
    int dr_count;
    bool verbose;

    CommandLineArguments() {
        domain_id  = 0;
        frame_count = 0;
        frame_rate = 60;
        sample_count = 1200;
        key = -1;
        size = 1048576;
        dr_count = 1;
        verbose = false;
    }
};

static void set_current_timestamp(RTIClock *clock, DDS_Time_t &timestamp)
{
    if (clock == NULL) {
        std::cout << "Clock not available" << std::endl;
        return;
    }
    RTINtpTime current_time = RTI_NTP_TIME_ZERO;
    clock->getTime(clock, &current_time);
    RTINtpTime_unpackToNanosec(
            timestamp.sec,
            timestamp.nanosec,
            current_time);
}

static void add_shmem_key_property(DDS::PropertyQosPolicy& property, int key)
{
    std::ostringstream key_to_str;
    key_to_str << key;

    DDS::PropertyQosPolicyHelper::assert_property(
            property,
            "shmem_key",
            key_to_str.str().c_str(),
            true);
}

static void fill_frame_data(Frame* frame)
{
    char *buffer = frame->get_buffer();
    int random_number = RTIOsapiUtility_rand();
    memcpy(buffer, &random_number, sizeof(random_number));

    // We dont compute CRC for the whole frame because it would add
    // a latency component not due to the middleware. Real
    // application may compute CRC differently
    frame->checksum = RTIOsapiUtility_crc32(buffer, sizeof(random_number), 0);
}

// Delete all entities
static int publisher_shutdown(
    DomainParticipant *participant)
{
    ReturnCode_t retcode;
    int status = 0;

    if (participant != NULL) {
        retcode = participant->delete_contained_entities();
        if (retcode != RETCODE_OK) {
            std::cout << "delete_contained_entities error " << retcode
                    << std::endl;
            status = -1;
        }
        retcode = TheParticipantFactory->delete_participant(participant);
        if (retcode != RETCODE_OK) {
            std::cout << "delete_participant error " << retcode << std::endl;
            status = -1;
        }
    }

    // RTI Connext provides finalize_instance() method on
    // domain participant factory for people who want to release memory used
    // by the participant factory. Uncomment the following block of code for
    // clean destruction of the singleton.

    //retcode = DomainParticipantFactory::finalize_instance();
    //if (retcode != RETCODE_OK) {
    //     std::cout << "finalize_instance error " << retcode << std::endl;
    //     status = -1;
    //}

    return status;
}

extern "C" int publisher_main(CommandLineArguments arg)
{
    // Creates a frame set in shared memory with the user provided settings
    FrameSet frame_set(
            arg.key, //shared memory key
            arg.frame_count, //number of frames contained in the shared memory segment
            arg.size //size of a frame
            );

    // To customize participant QoS, use
    // the configuration file USER_QOS_PROFILES.xml
    DomainParticipant *participant = TheParticipantFactory->create_participant(
        arg.domain_id, PARTICIPANT_QOS_DEFAULT,
        NULL /* listener */, STATUS_MASK_NONE);
    if (participant == NULL) {
        std::cout << "create_participant error" << std::endl;
        publisher_shutdown(participant);
        return -1;
    }

    // To customize publisher QoS, use
    // the configuration file USER_QOS_PROFILES.xml
    Publisher *publisher = participant->create_publisher(
        PUBLISHER_QOS_DEFAULT, NULL /* listener */, STATUS_MASK_NONE);
    if (publisher == NULL) {
        std::cout << "create_publisher error" << std::endl;
        publisher_shutdown(participant);
        return -1;
    }

    // Register type before creating topic
    const char *type_name = ZeroCopyTypeSupport::get_type_name();
    ReturnCode_t retcode = ZeroCopyTypeSupport::register_type(
        participant, type_name);
    if (retcode != RETCODE_OK) {
        std::cout << "register_type error " << retcode << std::endl;
        publisher_shutdown(participant);
        return -1;
    }

    // To customize topic QoS, use
    // the configuration file USER_QOS_PROFILES.xml
    Topic *topic = participant->create_topic(
        "Example ZeroCopy",
        type_name, TOPIC_QOS_DEFAULT, NULL /* listener */,
        STATUS_MASK_NONE);
    if (topic == NULL) {
        std::cout << "create_topic error" << std::endl;
        publisher_shutdown(participant);
        return -1;
    }

    // To customize data writer QoS, use
    // the configuration file USER_QOS_PROFILES.xml
    DDS::DataWriterQos writer_qos;
    publisher->get_default_datawriter_qos(writer_qos);

    // The key identifying the shared memory segment containing the frames
    // is sent to the Subscriber app so that it can attcah to it
    add_shmem_key_property(writer_qos.property, arg.key);

    DataWriter *writer = publisher->create_datawriter(
        topic, writer_qos, NULL /* listener */,
        STATUS_MASK_NONE);
    if (writer == NULL) {
        std::cout << "create_datawriter error" << std::endl;
        publisher_shutdown(participant);
        return -1;
    }
    ZeroCopyDataWriter *ZeroCopy_writer = ZeroCopyDataWriter::narrow(writer);
    if (ZeroCopy_writer == NULL) {
        std::cout << "DataWriter narrow error" << std::endl;
        publisher_shutdown(participant);
        return -1;
    }

    // Create data sample for writing
    ZeroCopy *instance = ZeroCopyTypeSupport::create_data();
    if (instance == NULL) {
        std::cout << "ZeroCopyTypeSupport::create_data error" << std::endl;
        publisher_shutdown(participant);
        return -1;
    }

    // Waiting to discover the expected number of DataReaders
    std::cout << "Waiting until " << arg.dr_count
            << " DataReaders are discovered" << std::endl;

    DDS_PublicationMatchedStatus matchedStatus;

    do {
        writer->get_publication_matched_status(matchedStatus);
    } while (matchedStatus.total_count < arg.dr_count);

    // Configuring the send period with frame rate
    Duration_t send_period = {0, 0};
    send_period.sec = 0;
    send_period.nanosec = 1000000000 / arg.frame_rate;

    // Create a clock to get the write timestamp
    RTIClock *clock = RTIHighResolutionClock_new();
    if (clock == NULL) {
        std::cout << "Failed to create the System clock" << std::endl;
        ZeroCopyTypeSupport::delete_data(instance);
        publisher_shutdown(participant);
        return -1;
    }

    // Main loop
    std::cout << "Going to send " << arg.sample_count << " frames at "
            << arg.frame_rate << " fps" << std::endl;

    DDS_WriteParams_t write_params = DDS_WRITEPARAMS_DEFAULT;
    int index = 0;
    for (int count = 0; count < arg.sample_count; ++count) {
        index = count % arg.frame_count;

        // Setting the source_timestamp before the shared memory segment is
        // updated
        set_current_timestamp(clock, write_params.source_timestamp);

        // Getting the frame at the desired index
        Frame *frame = frame_set[index];

        // Updating the frame with the desired data and setting the checksum
        frame->length = arg.size;
        frame->dimension.x = 800;
        frame->dimension.y = 1600;
        fill_frame_data(frame);

        if (arg.verbose) {
            std::cout << "Sending frame " << count + 1 << " with checksum: "
                    << frame->checksum << std::endl;
        }

        // Sending the index and checksum
        instance->index = index;
        instance->checksum = frame->checksum;
        retcode = ZeroCopy_writer->write_w_params(*instance, write_params);
        if (retcode != RETCODE_OK) {
            std::cout << "write error " << retcode << std::endl;
        }

        if ((count % arg.frame_rate) == (arg.frame_rate-1)) {
            std::cout << "Writen " << count+1 <<" frames" << std::endl;
        }

        NDDSUtility::sleep(send_period);
    }
    std::cout << "Written " << arg.sample_count << " frames" << std::endl;
    std::cout << "Publisher application shutting down" << std::endl;

    RTIHighResolutionClock_delete(clock);

    // Delete data sample
    retcode = ZeroCopyTypeSupport::delete_data(instance);
    if (retcode != RETCODE_OK) {
        std::cout << "ZeroCopyTypeSupport::delete_data error " << retcode
                << std::endl;
    }

    // Delete all entities
    return publisher_shutdown(participant);
}

void ZeroCopy_print_usage()
{
    std::cout << "Usage: ZeroCopy_publisher [options]" << std::endl;
    std::cout << "Where arguments are:" << std::endl;
    std::cout << "  -h                  Shows this page" << std::endl;
    std::cout << "  -d  <domain_id>     Sets the domain id [default = 0]" << std::endl;
    std::cout << "  -fc <frame count>   Sets the total number of frames to be mapped "
                                 "in the shared memory queue [default = frame rate]" << std::endl;
    std::cout << "  -fr <frame rate>    Sets the rate at which frames are written "
                                 "[default = 60fps]" << std::endl;
    std::cout << "  -sc <sample count>  Sets the number of frames to send "
                                 "[default = 1200]" << std::endl;
    std::cout << "  -k  <key>           Sets the key for shared memory segment "
                                 "[default = 10] " << std::endl;
    std::cout << "  -s  <buffer size>   Sets payload size of the frame in bytes "
                                 "[default = 1048576 (1MB)]" << std::endl;
    std::cout << "  -rc <dr count>      Expected number of DataReaders that will receive frames "
                                 "[default = 1]" << std::endl;
    std::cout << "  -v                  Displays the checksum for each frame " << std::endl;
}

int main(int argc, char *argv[])
{
    CommandLineArguments arg;

    // Parse the optional arguments
    for (int i = 1; i < argc; ++i) {
        if (!strncmp(argv[i], "-h", 2)) {
            ZeroCopy_print_usage();
            return 0;
        } else if (!strncmp(argv[i], "-d", 2)) {
            char *ptr;
            arg.domain_id = strtol(argv[++i], &ptr, 10);
        } else if (!strncmp(argv[i], "-fc", 3)) {
            char *ptr;
            arg.frame_count = strtol(argv[++i], &ptr, 10);
        } else if (!strncmp(argv[i], "-fr", 3)) {
            char *ptr;
            arg.frame_rate = strtol(argv[++i], &ptr, 10);
        } else if (!strncmp(argv[i], "-sc", 3)) {
            char *ptr;
            arg.sample_count = strtol(argv[++i], &ptr, 10);
        } else if (!strncmp(argv[i], "-k", 2)) {
            char *ptr;
            arg.key = strtol(argv[++i], &ptr, 10);
        } else if (!strncmp(argv[i], "-s", 2)) {
            char *ptr;
            arg.size = strtol(argv[++i], &ptr, 10);

            if (arg.size < 4) {
                std::cout << "-s must be greater or equal to 4" << std::endl;
                return -1;
            }
        } else if (!strncmp(argv[i], "-rc", 3)) {
            char *ptr;
            arg.dr_count = strtol(argv[++i], &ptr, 10);
        } else if (!strncmp(argv[i], "-v", 2)) {
            arg.verbose = true;
        } else {
            std::cout << "Invalid command-line parameter: " << argv[i]
                    << std::endl;
            return -1;
        }
    }

    if (arg.frame_count == 0) {
        arg.frame_count = arg.frame_rate;
    }

    if (arg.key == -1) {
        CommandLineArguments * argPtr = &arg;
        memcpy(&arg.key,&argPtr, sizeof(int));
    }

    std::cout << "Running using:" << std::endl;
    std::cout << "  Domain ID:  " << arg.domain_id << std::endl;
    std::cout << "  Frame Count in SHMEM segment: " << arg.frame_count << std::endl;
    std::cout << "  Frame Rate: " << arg.frame_rate << " fps" << std::endl;
    std::cout << "  Sample Count: " << arg.sample_count << std::endl;
    std::cout << "  SHMEM Key: " << arg.key << std::endl;
    std::cout << "  Frame size: " << arg.size << " bytes" << std::endl;
    std::cout << "  Expected DataReader Count: " << arg.dr_count << std::endl;

    // Uncomment this to turn on additional logging
    // NDDSConfigLogger::get_instance()->
    //     set_verbosity_by_category(NDDS_CONFIG_LOG_CATEGORY_API,
    //     NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL);


    return publisher_main(arg);
}

