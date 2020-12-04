/*
* (c) Copyright, Real-Time Innovations, 2020.  All rights reserved.
* RTI grants Licensee a license to use, modify, compile, and create derivative
* works of the software solely for use with RTI Connext DDS. Licensee may
* redistribute copies of the software provided that all such copies are subject
* to this license. The software is provided "as is", with no warranty of any
* type, including any warranty for fitness for any purpose. RTI is under no
* obligation to maintain or support the software. RTI shall not be liable for
* any incidental or consequential damages arising out of the use or inability
* to use the software.
*/

/*
 * A simple HelloWorld using network capture to save DomainParticipant traffic.
 *
 * This example is a simple hello world running network capture for both a
 * publisher participant (example0_publisher.cxx).and a subscriber participant
 * (example0_subscriber.cxx).
 * It shows the basic flow when working with network capture:
 *   - Enabling before anything else.
 *   - Start capturing traffic (for one or all participants).
 *   - Pause/resume capturing traffic (for one or all participants).
 *   - Stop capturing trafffic (for one or all participants).
 *   - Disable after everything else.
 */

#include <stdio.h>
#include <stdlib.h>

#include "example0.h"
#include "example0Support.h"
#include "ndds/ndds_cpp.h"

static int publisher_shutdown(
        DDSDomainParticipant *participant)
{
    DDS_ReturnCode_t retcode;
    int status = 0;

    if (participant != NULL) {
        retcode = participant->delete_contained_entities();
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "delete_contained_entities error %d\n", retcode);
            status = -1;
        }

        retcode = DDSTheParticipantFactory->delete_participant(participant);
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "delete_participant error %d\n", retcode);
            status = -1;
        }
    }

    retcode = DDSDomainParticipantFactory::finalize_instance();
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "finalize_instance error %d\n", retcode);
        status = -1;
    }

    /*
     * Disable network capture.
     *
     * This must be:
     *   - The last network capture function that is called.
     */
    if (!NDDSUtilityNetworkCapture::disable()) {
        fprintf(stderr, "Error disabling network capture");
        status = -1;
    }

    return status;
}

extern "C" int publisher_main(int domainId, int sample_count)
{
    DDSDomainParticipant *participant = NULL;
    DDSPublisher *publisher = NULL;
    DDSTopic *topic = NULL;
    DDSDataWriter *writer = NULL;
    example0DataWriter * example0_writer = NULL;
    example0 *instance = NULL;
    DDS_ReturnCode_t retcode;
    DDS_InstanceHandle_t instance_handle = DDS_HANDLE_NIL;
    const char *type_name = NULL;
    int count = 0;
    DDS_Duration_t send_period = {1, 0};

    /*
     * Enable network capture.
     *
     * This must be called before:
     *   - Any other network capture function is called.
     *   - Creating the participants for which we want to capture traffic.
     */
    if (!NDDSUtilityNetworkCapture::enable()) {
        fprintf(stderr, "Error enabling network capture");
        return -1;
    }

    participant = DDSTheParticipantFactory->create_participant(
            domainId,
            DDS_PARTICIPANT_QOS_DEFAULT,
            NULL, /* listener */
            DDS_STATUS_MASK_NONE);
    if (participant == NULL) {
        fprintf(stderr, "create_participant error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /*
     * Start capturing traffic for all participants.
     *
     * All participants: those already created and those yet to be created.
     * Default parameters: all transports and some other sane defaults.
     *
     * A capture file will be created for each participant. The capture file
     * will start with the prefix "publisher" and continue with a suffix
     * dependent on the participant's GUID.
     */
    if (!NDDSUtilityNetworkCapture::start("publisher")) {
        fprintf(stderr, "Error starting network capture");
        return -1;
    }

    /* The example continues as the usual Connext DDS HelloWorld. */
    publisher = participant->create_publisher(
            DDS_PUBLISHER_QOS_DEFAULT,
            NULL, /* listener */
            DDS_STATUS_MASK_NONE);
    if (publisher == NULL) {
        fprintf(stderr, "create_publisher error\n");
        publisher_shutdown(participant);
        return -1;
    }

    type_name = example0TypeSupport::get_type_name();
    retcode = example0TypeSupport::register_type(
            participant,
            type_name);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "register_type error %d\n", retcode);
        publisher_shutdown(participant);
        return -1;
    }

    topic = participant->create_topic(
            "Example0 using network capture C++ API",
            type_name,
            DDS_TOPIC_QOS_DEFAULT,
            NULL, /* listener */
            DDS_STATUS_MASK_NONE);
    if (topic == NULL) {
        fprintf(stderr, "create_topic error\n");
        publisher_shutdown(participant);
        return -1;
    }

    writer = publisher->create_datawriter(
            topic,
            DDS_DATAWRITER_QOS_DEFAULT,
            NULL, /* listener */
            DDS_STATUS_MASK_NONE);
    if (writer == NULL) {
        fprintf(stderr, "create_datawriter error\n");
        publisher_shutdown(participant);
        return -1;
    }
    example0_writer = example0DataWriter::narrow(writer);
    if (example0_writer == NULL) {
        fprintf(stderr, "DataWriter narrow error\n");
        publisher_shutdown(participant);
        return -1;
    }

    instance = example0TypeSupport::create_data();
    if (instance == NULL) {
        fprintf(stderr, "example0TypeSupport::create_data error\n");
        publisher_shutdown(participant);
        return -1;
    }

    for (count=0; (sample_count == 0) || (count < sample_count); ++count) {

        printf("Writing example0, count %d\n", count);
        snprintf(instance->msg, 128, "Hello World (%d)", count);

        /*
         * Here we are going to pause capturing for some samples.
         * The resulting pcap file will not contain them.
         */
        if (count == 4 && !NDDSUtilityNetworkCapture::pause()) {
            fprintf(stderr, "Error pausing network capture");
            return -1;
        } else if (count == 6 && !NDDSUtilityNetworkCapture::resume()) {
            fprintf(stderr, "Error resuming network capture");
            return -1;
        }

        retcode = example0_writer->write(*instance, instance_handle);
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "write error %d\n", retcode);
        }

        NDDSUtility::sleep(send_period);
    }

    retcode = example0_writer->unregister_instance(
            *instance,
            instance_handle);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "unregister instance error %d\n", retcode);
    }

    retcode = example0TypeSupport::delete_data(instance);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "example0TypeSupport::delete_data error %d\n", retcode);
    }

    /*
     * Before deleting the participants that are capturing, we must stop
     * network capture for them.
     */
    if (!NDDSUtilityNetworkCapture::stop()) {
        fprintf(stderr, "Error stopping network capture");
        return -1;
    }
    return publisher_shutdown(participant);
}

int main(int argc, char *argv[])
{
    int domain_id = 0;
    int sample_count = 0; /* infinite loop */

    if (argc >= 2) {
        domain_id = atoi(argv[1]);
    }
    if (argc >= 3) {
        sample_count = atoi(argv[2]);
    }

    return publisher_main(domain_id, sample_count);
}

