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
        const StreamInfo &info,
        const PropertySet &properties,
        StreamReaderListener *listener)
        : samplingPeriod_(50),
          streamInfo_(info.stream_name(), info.type_info().type_name())
{
    connection_ = connection;
    readerListener_ = listener;
    adapterType_ =
            static_cast<DynamicType *>(info.type_info().type_representation());

    connReader_ = new ConnectorMsqlDb(info.stream_name(), properties);
    if (connReader_->connected()) {
        readerThread_ = std::thread(&RsStreamReader::readDataThread, this);
    }
}

RsStreamReader::~RsStreamReader()
{
    shutdownReaderThread();
    delete connReader_;
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
     * function readDataThread()
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

void RsStreamReader::readDataThread()
{
    RTI_RS_LOG_FN(readDataThread);

    while (!stopThread_) {
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
            sample_.reset(new DynamicData(*adapterType_));

            /* read data from the input connector */
            numRecords = connReader_->readData(sample_);
        }
        if (numRecords > 0) {
            readerListener_->on_data_available(this);
            std::this_thread::sleep_for(samplingPeriod_);
        } else {
            stopThread_ = true;
        }
    }
    std::cout << "Reached end of " << streamInfo_.stream_name()
              << " database records" << std::endl;
    connection_->dispose_discovery_stream(streamInfo_);
}

void RsStreamReader::shutdownReaderThread()
{
    RTI_RS_LOG_FN(shutdownReaderThread);

    stopThread_ = true;
    readerThread_.join();
}
