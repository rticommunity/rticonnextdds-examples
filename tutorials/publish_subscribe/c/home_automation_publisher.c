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
#include "home_automation.h"
#include "home_automationSupport.h"

static int publisher_shutdown(DDS_DomainParticipant *participant);

int publish_sensor(const char *sensor_name, const char *room_name)
{
    DDS_DomainParticipant *participant = NULL;
    DDS_Topic *topic = NULL;
    DDS_DataWriter *writer = NULL;
    DeviceStatusDataWriter *DeviceStatus_writer = NULL;
    const char *type_name = NULL;
    DeviceStatus *device_status = NULL;
    DDS_ReturnCode_t retcode;
    struct DDS_Duration_t send_period = { 2, 0 };
    int i;

    participant = DDS_DomainParticipantFactory_create_participant(
            DDS_TheParticipantFactory,
            0,
            &DDS_PARTICIPANT_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);

    if (participant == NULL) {
        fprintf(stderr, "create_participant error\n");
        publisher_shutdown(participant);
        return -1;
    }

    type_name = DeviceStatusTypeSupport_get_type_name();
    retcode = DeviceStatusTypeSupport_register_type(participant, type_name);

    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "register_type error %d\n", retcode);
        publisher_shutdown(participant);
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
        publisher_shutdown(participant);
        return -1;
    }

    writer = DDS_DomainParticipant_create_datawriter(
            participant,
            topic,
            &DDS_DATAWRITER_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);

    if (writer == NULL) {
        fprintf(stderr, "create_datawriter error\n");
        publisher_shutdown(participant);
        return -1;
    }

    DeviceStatus_writer = DeviceStatusDataWriter_narrow(writer);
    if (DeviceStatus_writer == NULL) {
        fprintf(stderr, "DataWriter narrow error\n");
        publisher_shutdown(participant);
        return -1;
    }

    device_status = DeviceStatusTypeSupport_create_data();
    if (device_status == NULL) {
        fprintf(stderr, "DeviceStatusTypeSupport_create_data error\n");
        publisher_shutdown(participant);
        return -1;
    }

    strncpy(device_status->sensor_name, sensor_name, 255);
    strncpy(device_status->room_name, room_name, 255);
    device_status->is_open = DDS_BOOLEAN_FALSE;

    for (i = 0; i < 1000; i++) {
        device_status->is_open =
                device_status->is_open ? DDS_BOOLEAN_FALSE : DDS_BOOLEAN_TRUE;
        retcode = DeviceStatusDataWriter_write(
                DeviceStatus_writer,
                device_status,
                &DDS_HANDLE_NIL);
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "write error %d\n", retcode);
        }

        NDDS_Utility_sleep(&send_period);
    }

    retcode = DeviceStatusTypeSupport_delete_data(device_status);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr,
                "DeviceStatusTypeSupport_delete_data error %d\n",
                retcode);
    }

    return publisher_shutdown(participant);
}

static int publisher_shutdown(DDS_DomainParticipant *participant)
{
    DDS_ReturnCode_t retcode;
    int status = 0;

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
    char *sensor_name = "Window1";
    char *room_name = "LivingRoom";

    if (argc > 1) {
        sensor_name = argv[1];
    }

    if (argc > 2) {
        room_name = argv[2];
    }

    return publish_sensor(sensor_name, room_name);
}
