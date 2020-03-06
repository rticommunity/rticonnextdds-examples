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

#ifndef RS_STREAM_READER_
#define RS_STREAM_READER_

#include "ConnectorMsqlDb.hpp"
#include "RsConnection.hpp"
#include <chrono>
#include <dds/core/xtypes/DynamicData.hpp>
#include <dds/sub/SampleInfo.hpp>
#include <mutex>
#include <rti/routing/adapter/StreamReader.hpp>
#include <thread>

class RsStreamReader : public rti::routing::adapter::DynamicDataStreamReader {
public:
    RsStreamReader(
            RsConnection *connection,
            const rti::routing::StreamInfo &stream_info,
            const rti::routing::PropertySet &properties,
            rti::routing::adapter::StreamReaderListener *listener);

    virtual ~RsStreamReader();

    void
            take(std::vector<dds::core::xtypes::DynamicData *> &samples,
                 std::vector<dds::sub::SampleInfo *> &infos) final;

    void take(
            std::vector<dds::core::xtypes::DynamicData *> &samples,
            std::vector<dds::sub::SampleInfo *> &infos,
            const rti::routing::adapter::SelectorState &selector_state) final;

    void return_loan(
            std::vector<dds::core::xtypes::DynamicData *> &samples,
            std::vector<dds::sub::SampleInfo *> &infos) final;

private:
    void read_data_thread();
    void shutdown_reader_thread();

    rti::routing::adapter::StreamReaderListener *reader_listener_;
    rti::routing::StreamInfo stream_info_;
    dds::core::xtypes::DynamicType *adapter_type_;

    RsConnection *connection_;
    std::thread reader_thread_;
    std::chrono::milliseconds sampling_period_; /* period to read data */
    bool stop_thread_ { false };

    ConnectorBase *conn_reader_ { nullptr };
    std::mutex data_mutex_;

    std::unique_ptr<dds::core::xtypes::DynamicData> sample_;
};

#endif /* RS_STREAM_READER_ */
