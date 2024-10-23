/*
 * (c) 2024 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 *  RTI grants Licensee a license to use, modify, compile, and create derivative
 *  works of the Software.  Licensee has the right to distribute object form
 *  only for use with RTI products.  The Software is provided "as is", with no
 *  warranty of any type, including any warranty for fitness for any purpose.
 *  RTI is under no obligation to maintain or support the Software.  RTI shall
 *  not be liable for any incidental or consequential damages arising out of the
 *  use or inability to use the software.
 */

#include "ndds/ndds_c.h"
#include "dds_c/dds_c_sample_processor.h"
#include "home_automation.h"
#include "home_automationSupport.h"


void DeviceStatusListener_on_new_sample(
        void *handler_data,
        const void *sample_data,
        const struct DDS_SampleInfo *sample_info)
{
    DeviceStatus *device_status = (DeviceStatus *) sample_data;

    if (sample_info->valid_data) {
        if (device_status->is_open) {
            printf("WARNING: %s in %s is open!\n",
                   device_status->sensor_name,
                   device_status->room_name);
        }
    }
}

static int subscriber_shutdown(
        DDS_DomainParticipant *participant,
        DDS_SampleProcessor *sample_processor);

int monitor_sensor(void)
{
    DDS_DomainParticipant *participant = NULL;
    DDS_Topic *topic = NULL;
    DDS_DataReader *reader = NULL;
    DDS_SampleProcessor *sample_processor = NULL;
    struct DDS_SampleHandler sample_handler = DDS_SampleHandler_INITIALIZER;
    const char *type_name = NULL;
    DDS_ReturnCode_t retcode;
    struct DDS_Duration_t poll_period = { 2, 0 };
    int i;

    participant = DDS_DomainParticipantFactory_create_participant(
            DDS_TheParticipantFactory,
            0,
            &DDS_PARTICIPANT_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);

    if (participant == NULL) {
        fprintf(stderr, "create_participant error\n");
        subscriber_shutdown(participant, sample_processor);
        return -1;
    }

    type_name = DeviceStatusTypeSupport_get_type_name();
    retcode = DeviceStatusTypeSupport_register_type(participant, type_name);

    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "register_type error %d\n", retcode);
        subscriber_shutdown(participant, sample_processor);
        return -1;
    }

    topic = DDS_DomainParticipant_create_topic(
            participant,
            "WindowStatus",
            type_name,
            &DDS_TOPIC_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);

    if (topic == NULL) {
        fprintf(stderr, "create_topic error\n");
        subscriber_shutdown(participant, sample_processor);
        return -1;
    }

    reader = DDS_DomainParticipant_create_datareader(
            participant,
            DDS_Topic_as_topicdescription(topic),
            &DDS_DATAREADER_QOS_DEFAULT,
            NULL, /* listener */
            DDS_STATUS_MASK_NONE);
    if (reader == NULL) {
        fprintf(stderr, "create_datareader error\n");
        subscriber_shutdown(participant, sample_processor);
        return -1;
    }

    sample_processor =
            DDS_SampleProcessor_new(&DDS_ASYNC_WAITSET_PROPERTY_DEFAULT);

    sample_handler.on_new_sample = DeviceStatusListener_on_new_sample;

    retcode = DDS_SampleProcessor_attach_reader(
            sample_processor,
            reader,
            &sample_handler);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "Failed to attach reader\n");
        subscriber_shutdown(participant, sample_processor);
        return -1;
    }

    for (i = 0; i < 1000; i++) {
        NDDS_Utility_sleep(&poll_period);
    }

    return subscriber_shutdown(participant, sample_processor);
}

static int subscriber_shutdown(
        DDS_DomainParticipant *participant,
        DDS_SampleProcessor *sample_processor)
{
    DDS_ReturnCode_t retcode;
    int status = 0;

    if (sample_processor != NULL) {
        retcode = DDS_SampleProcessor_delete(sample_processor);
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "Failed to delete sample processor\n");
            status = -1;
        }
    }

    if (participant != NULL) {
        retcode = DDS_DomainParticipant_delete_contained_entities(participant);
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "delete_contained_entities error %d\n", retcode);
            status = -1;
        }

        retcode = DDS_DomainParticipantFactory_delete_participant(
                DDS_TheParticipantFactory,
                participant);
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "delete_participant error %d\n", retcode);
            status = -1;
        }
    }

    return status;
}

int main(int argc, char **argv)
{
    return monitor_sensor();
}
