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

#include <cstdio>
#include <cstdlib>
#include <map>
#include <iostream>
#include <sstream>

#include "zero_copy.h"
#include "zero_copySupport.h"
#include "FrameSupport.h"
#include "ndds/ndds_cpp.h"
#include "ndds/ndds_namespace_cpp.h"
#include "ndds/clock/clock_highResolution.h"
#include "ndds/osapi/osapi_ntptime.h"

using namespace DDS;

struct CommandLineArguments {
public:
    int domain_id;
    int size;
    int sample_count;
    bool verbose;

    CommandLineArguments() {
        domain_id  = 0;
        sample_count = 1200;
        size = 1048576;
        verbose = false;
    }
};
CommandLineArguments arg;

// Compute the latency with the given current_time and source_timestamp
static void compute_latency(
        DDS_Duration_t &latency,
        RTINtpTime current_time,
        const DDS_Time_t &source_timestamp)
{
    RTINtpTime source_time = RTI_NTP_TIME_ZERO;

    // Convert DDS_Time_t format to RTINtpTime format
    RTINtpTime_packFromNanosec(
            source_time,
            source_timestamp.sec,
            source_timestamp.nanosec);

    // Compute the difference and store in current_time
    RTINtpTime_decrement(current_time, source_time);

    // Convert RTINtpTime format to DDS_Duration_t format
    RTINtpTime_unpackToNanosec(latency.sec, latency.nanosec, current_time);
}

// Get the shared memory key from the publication data property
static int lookup_shmem_key_property(DDS::PropertyQosPolicy& property)
{
    const DDS::Property_t *p = DDS::PropertyQosPolicyHelper::lookup_property(
            property,
            "shmem_key");
    if (p == NULL) {
        return -1;
    }

    int key;
    std::istringstream str_to_key(p->value);
    str_to_key >> key;

    return key;
}

bool operator<(const DDS::GUID_t& a, const DDS::GUID_t& b)
{
    return DDS_GUID_compare(&a, &b) < 0;
}

bool operator==(const DDS::GUID_t& a, const DDS::GUID_t& b)
{
    return DDS_GUID_equals(&a, &b) == DDS_BOOLEAN_TRUE;
}

class ShmemKeyTrackingReaderListener : public DDSDataReaderListener {

public:
    ~ShmemKeyTrackingReaderListener()
    {
        for (FrameSetViewMap::iterator iter = frame_set_views.begin();
                iter != frame_set_views.end(); ++iter) {
            delete iter->second;
        }
    }

    FrameSetView* get_frame_set_view(const DDS::GUID_t& publication_guid) const
    {
        FrameSetViewMap::const_iterator key_it = frame_set_views.find(
                publication_guid);
        if (key_it == frame_set_views.end()) {
            return NULL;
        }

        return key_it->second;
    }

    size_t get_frame_set_view_count() const
    {
        return frame_set_views.size();
    }

private:
    typedef std::map<DDS::GUID_t, FrameSetView*> FrameSetViewMap;

    // Map to store the frame_set_views created for each matched frame_set
    FrameSetViewMap frame_set_views;

    void on_subscription_matched(
        DDSDataReader* reader,
        const DDS_SubscriptionMatchedStatus& status) /* override */
    {
        try {
            DDS::PublicationBuiltinTopicData publication_data;

            if (reader->get_matched_publication_data(
                    publication_data,
                    status.last_publication_handle) != DDS_RETCODE_OK) {
                return;
            }

            int key = lookup_shmem_key_property(publication_data.property);

            if (key == -1) {
                return;
            }

            DDS::GUID_t publication_guid;
            DDS_BuiltinTopicKey_to_guid(
                    &publication_data.key,
                    &publication_guid);
            if (frame_set_views.find(publication_guid)
                    == frame_set_views.end()){
                // Create the frame_set_view with the key and size. This frame
                // set view attaches to the shared memory segment created by the
                // frame set managed by the matched DataWriter
                FrameSetView *frame_set_view = new FrameSetView(key, arg.size);
                frame_set_views[publication_guid] = frame_set_view;
            } else {
                std::cout << "Unable to track the key " << key << std::endl;
            }
        } catch (...) {
            std::cout << "Exception in on_subscription_matched. " << std::endl;
        }

    }
};

class ZeroCopyListener : public ShmemKeyTrackingReaderListener {
  public:
    virtual void on_requested_deadline_missed(
        DataReader* /*reader*/,
        const RequestedDeadlineMissedStatus& /*status*/) {}

    virtual void on_requested_incompatible_qos(
        DataReader* /*reader*/,
        const RequestedIncompatibleQosStatus& /*status*/) {}

    virtual void on_sample_rejected(
        DataReader* /*reader*/,
        const SampleRejectedStatus& /*status*/) {}

    virtual void on_liveliness_changed(
        DataReader* /*reader*/,
        const LivelinessChangedStatus& /*status*/) {}

    virtual void on_sample_lost(
        DataReader* /*reader*/,
        const SampleLostStatus& /*status*/) {}

    virtual void on_data_available(DataReader* reader);

    ZeroCopyListener() :
        total_latency(DDS_DURATION_ZERO),
        received_frames(0),
        shutdown_application(false)
    {
        clock = RTIHighResolutionClock_new();
    }
    ~ZeroCopyListener()
    {
        RTIHighResolutionClock_delete(clock);
    }
    bool start_shutdown()
    {
        return shutdown_application;
    }
  private:
    // Sum of frame latency
    DDS_Duration_t total_latency;
    // Total number of received frames
    int received_frames;
    // Used for getting system time
    RTIClock *clock;
    RTINtpTime previous_print_time;
    bool shutdown_application;

    // Prints average latency in micro seconds
    void print_average_latency()
    {
        std::cout << "Average latency: "
                << ((total_latency.sec * 1000000)
                        + (total_latency.nanosec / 1000)) / received_frames
                << " microseconds" << std::endl;
    }
};

void ZeroCopyListener::on_data_available(DataReader* reader)
{
    ZeroCopyDataReader *ZeroCopy_reader = ZeroCopyDataReader::narrow(reader);
    if (ZeroCopy_reader == NULL) {
        std::cout << "DataReader narrow error" << std::endl;
        return;
    }
    ZeroCopySeq data_seq;
    SampleInfoSeq info_seq;
    ReturnCode_t retcode;
    retcode = ZeroCopy_reader->take(
        data_seq, info_seq, LENGTH_UNLIMITED,
        ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

    if (retcode == RETCODE_NO_DATA) {
        return;
    } else if (retcode != RETCODE_OK) {
        std::cout << "take error "<< retcode << std::endl;
        return;
    }

    unsigned int received_checksum = 0, computed_checksum = 0;
    RTINtpTime current_time = RTI_NTP_TIME_ZERO;
    DDS_Duration_t latency;

    for (int i = 0; i < data_seq.length(); ++i) {
        if (info_seq[i].valid_data) {

            // Get the frame_set_view associated with the virtual guid
            // This is done because there may be multiple Publishers
            // publishing frames. Each publisher creates its own
            // shared memory segement to store frames and is identified by
            // info_seq[i].publication_virtual_guid
            FrameSetView *frame_set_view = get_frame_set_view(
                    info_seq[i].publication_virtual_guid);

            if (frame_set_view == NULL) {
                std::cout << "Received data from an un recognized frame "
                        "writer" << std::endl;
                continue;
            }

            // Get the frame checksum from the sample
            received_checksum = data_seq[i].checksum;

            // Get the frame from the shared memory segment
            // at the provided index
            const Frame *frame = (*frame_set_view)[data_seq[i].index];

            // Compute the checksum of the frame
            // We only compute the checksum of the first 4 bytes. These
            // are the bytes that we change in each frame for this example

            // We dont compute CRC for the whole frame because it would add
            // a latency component not due to the middleware. Real
            // application may compute CRC differently
            computed_checksum = RTIOsapiUtility_crc32(
                    frame->get_buffer(),
                    sizeof(int),
                    0);

            // Validate the computed checksum with the checksum sent by the
            // DataWriter
            if (computed_checksum != received_checksum) {
                std::cout << "Checksum NOT OK" << std::endl;
            }

            // Compute the latency from the source time
            clock->getTime(clock, &current_time);
            compute_latency(latency, current_time, info_seq[i].source_timestamp);

            // Keep track of total latency for computing averages
            total_latency = total_latency + latency;
            received_frames++;

            if (arg.verbose) {
                std::cout << "Received frame " << received_frames
                        << " from DataWriter with key "
                        << frame_set_view->get_key() << " with checksum: "
                        << frame->checksum << std::endl;
            }

            if (received_frames == 1) {
                previous_print_time = current_time;
            }

            if (((current_time.sec - previous_print_time.sec) >= 1)
                    || (received_frames == arg.sample_count)) {
                previous_print_time = current_time;
                print_average_latency();
            }
        }
    }

    retcode = ZeroCopy_reader->return_loan(data_seq, info_seq);
    if (retcode != RETCODE_OK) {
        std::cout << "return loan error " << retcode << std::endl;
    }
    if ((received_frames == arg.sample_count) && (!shutdown_application)) {
        std::cout << "Received " << received_frames << " frames " << std::endl;
        shutdown_application = true;
    }
}

// Delete all entities
static int subscriber_shutdown(
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

    // RTI Connext provides the finalize_instance() method on
    // domain participant factory for people who want to release memory used
    // by the participant factory. Uncomment the following block of code for
    // clean destruction of the singleton.


    // retcode = DomainParticipantFactory::finalize_instance();
    // if (retcode != RETCODE_OK) {
    //     std::cout << "finalize_instance error " << retcode << std::endl;
    //     status = -1;
    // }

    return status;
}

extern "C" int subscriber_main()
{
    // To customize the participant QoS, use
    // the configuration file USER_QOS_PROFILES.xml
    DomainParticipant *participant = TheParticipantFactory->create_participant(
        arg.domain_id, PARTICIPANT_QOS_DEFAULT,
        NULL /* listener */, STATUS_MASK_NONE);
    if (participant == NULL) {
        std::cout << "create_participant error" << std::endl;
        subscriber_shutdown(participant);
        return -1;
    }

    // To customize the subscriber QoS, use
    // the configuration file USER_QOS_PROFILES.xml
    Subscriber *subscriber = participant->create_subscriber(
        SUBSCRIBER_QOS_DEFAULT, NULL /* listener */, STATUS_MASK_NONE);
    if (subscriber == NULL) {
        std::cout << "create_subscriber error" << std::endl;
        subscriber_shutdown(participant);
        return -1;
    }

    // Register the type before creating the topic
    const char *type_name = ZeroCopyTypeSupport::get_type_name();
    ReturnCode_t retcode = ZeroCopyTypeSupport::register_type(
        participant, type_name);
    if (retcode != RETCODE_OK) {
        std::cout << "register_type error " << retcode << std::endl;
        subscriber_shutdown(participant);
        return -1;
    }

    // To customize the topic QoS, use
    // the configuration file USER_QOS_PROFILES.xml
    Topic *topic = participant->create_topic(
        "Example ZeroCopy",
        type_name, TOPIC_QOS_DEFAULT, NULL /* listener */,
        STATUS_MASK_NONE);
    if (topic == NULL) {
        std::cout << "create_topic error" << std::endl;
        subscriber_shutdown(participant);
        return -1;
    }

    // Create a data reader listener
    ZeroCopyListener *reader_listener = new ZeroCopyListener();

    // To customize the data reader QoS, use
    // the configuration file USER_QOS_PROFILES.xml
    DataReader *reader = subscriber->create_datareader(
        topic, DATAREADER_QOS_DEFAULT, reader_listener,
        STATUS_MASK_ALL);
    if (reader == NULL) {
        std::cout << "create_datareader error" << std::endl;
        subscriber_shutdown(participant);
        delete reader_listener;
        return -1;
    }

    // Main loop
    std::cout << "ZeroCopy subscriber waiting to receive samples..."
            << std::endl;
    Duration_t receive_period = {4,0};
    while (!reader_listener->start_shutdown()) {
        NDDSUtility::sleep(receive_period);
    };
    std::cout << "Subscriber application shutting down" << std::endl;
    // Delete all entities
    int status = subscriber_shutdown(participant);
    delete reader_listener;

    return status;
}

void ZeroCopy_print_usage()
{
    std::cout << "Usage: ZeroCopy_subscriber [options]" << std::endl;
    std::cout << "Where arguments are:" << std::endl;
    std::cout << "  -h                  Shows this page" << std::endl;
    std::cout << "  -d  <domain_id>     Sets the domain id [default = 0]" << std::endl;
    std::cout << "  -sc <sample count>  Sets the number of frames to receive "
                                 "[default = 1200]" << std::endl;
    std::cout << "  -s  <buffer size>   Sets the payload size of the frame in bytes "
                                 "[default = 1048576 (1MB)]" << std::endl;
    std::cout << "  -v                  Displays checksum and computed latency of "
                                 "each received frame " << std::endl;

}

int main(int argc, char *argv[])
{
    // Parse the optional arguments
    for (int i = 1; i < argc; ++i) {
        if (!strncmp(argv[i], "-h", 2)) {
            ZeroCopy_print_usage();
            return 0;
        } else if (!strncmp(argv[i], "-d", 2)) {
            char *ptr;
            arg.domain_id = strtol(argv[++i], &ptr, 10);
        } else if (!strncmp(argv[i], "-sc", 3)) {
            char *ptr;
            arg.sample_count = strtol(argv[++i], &ptr, 10);
        } else if (!strncmp(argv[i], "-s", 2)) {
            char *ptr;
            arg.size = strtol(argv[++i], &ptr, 10);
        } else if (!strncmp(argv[i], "-v", 2)) {
            arg.verbose = true;
        } else {
            std::cout << "Invalid command-line parameter: " << argv[i] << std::endl;
            return -1;
        }
    }

    std::cout << "Running using:" << std::endl;
    std::cout << "  Domain ID:  " << arg.domain_id << std::endl;
    std::cout << "  Sample Count: " << arg.sample_count << std::endl;
    std::cout << "  Frame size: " << arg.size <<  " bytes" << std::endl;

    // Uncomment this to turn on additional logging
    // NDDSConfigLogger::get_instance()->
    //     set_verbosity_by_category(NDDS_CONFIG_LOG_CATEGORY_API,
    //     NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL);


    return subscriber_main();
}

