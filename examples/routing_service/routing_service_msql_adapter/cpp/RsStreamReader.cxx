/*
 * (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software.  Licensee has the right to distribute object form
 * only for use with RTI products.  The Software is provided "as is", with no
 * warranty of any type, including any warranty for fitness for any purpose.
 * RTI is under no obligation to maintain or support the Software.  RTI shall
 * not be liable for any incidental or consequential damages arising out of the
 * use or inability to use the software.
 */

#include "RsStreamReader.hpp"
#include "RsLog.hpp"

#define RTI_RS_LOG_ARGS "RsStreamReader"

using namespace rti::routing::adapter;
using namespace rti::routing;
using namespace dds::core::xtypes;
using namespace dds::sub;

RsStreamReader::RsStreamReader(
        RsConnection *connection,
        const StreamInfo &stream_info,
        const PropertySet &properties,
        StreamReaderListener *listener)
        : sampling_period_(50),
          stream_info_(
                  stream_info.stream_name(),
                  stream_info.type_info().type_name()),
          connection_(connection),
          reader_listener_(listener),
          adapter_type_(static_cast<DynamicType *>(
                  stream_info.type_info().type_representation())),
          conn_reader_(
                  new ConnectorMsqlDb(stream_info.stream_name(), properties))
{
    if (conn_reader_->connected()) {
        reader_thread_ = std::thread(&RsStreamReader::read_data_thread, this);
    }
}

RsStreamReader::~RsStreamReader()
{
    shutdown_reader_thread();
    delete conn_reader_;
}

void RsStreamReader::take(
        std::vector<DynamicData *> &samples,
        std::vector<SampleInfo *> &infos)
{
    RTI_RS_LOG_FN(take);

    /*
     * This protection is required since take() executes on a different
     * Routing Service thread.
     */
    std::lock_guard<std::mutex> guard(data_mutex_);

    /*
     * Note that we read one record at a time from the in the
     * function read_data_thread()
     */
    samples.resize(1);
    infos.resize(1);

    samples[0] = sample_.release();
}

void RsStreamReader::take(
        std::vector<DynamicData *> &samples,
        std::vector<SampleInfo *> &infos,
        const SelectorState &selector_state)
{
    RTI_RS_LOG_FN(take_with_selector_state);

    /* Selector ignored for now. Just call take.*/
    take(samples, infos);
}

void RsStreamReader::return_loan(
        std::vector<DynamicData *> &samples,
        std::vector<SampleInfo *> &infos)
{
    RTI_RS_LOG_FN(return_loan);

    for (int i = 0; i < samples.size(); ++i) {
        delete samples[i];
        delete infos[i];
    }
    samples.clear();
    infos.clear();
}

void RsStreamReader::read_data_thread()
{
    RTI_RS_LOG_FN(read_data_thread);

    while (!stop_thread_) {
        unsigned int numRecords = 0;
        {
            /*
             * Essential to protect against concurrent data access to
             * shapeData from the take() methods running on a different
             * Routing Service thread.
             */
            std::lock_guard<std::mutex> guard(data_mutex_);

            /* Destroys the object currently managed by the unique_ptr (if any)
             * and takes ownership */
            sample_.reset(new DynamicData(*adapter_type_));

            /* read data from the input connector */
            numRecords = conn_reader_->read_data(sample_);
        }
        if (numRecords > 0) {
            reader_listener_->on_data_available(this);
            std::this_thread::sleep_for(sampling_period_);
        } else {
            stop_thread_ = true;
        }
    }
    std::cout << "Reached end of " << stream_info_.stream_name()
              << " database records" << std::endl;
    connection_->dispose_discovery_stream(stream_info_);
}

void RsStreamReader::shutdown_reader_thread()
{
    RTI_RS_LOG_FN(shutdown_reader_thread);

    stop_thread_ = true;
    reader_thread_.join();
}
