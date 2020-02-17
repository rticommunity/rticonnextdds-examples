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

#include "RsInputDiscoveryStreamReader.hpp"
#include "RsLog.hpp"

#define RTI_RS_LOG_ARGS "RsInputDiscoveryStreamReader"

using namespace rti::routing;
using namespace rti::routing::adapter;

/*
 * The stream type and stream names are hard coded.
 */
constexpr auto RTI_STREAM_TYPE = "ShapeType";
constexpr auto SQUARE = "Square";
constexpr auto CIRCLE = "Circle";
constexpr auto TRIANGLE = "Triangle";

/*
 * This class implements a DiscoveryStreamReader, a special kind of StreamReader
 * that provide discovery information about the available streams and their
 * types.
 */

RsInputDiscoveryStreamReader::RsInputDiscoveryStreamReader(
        const PropertySet &,
        StreamReaderListener *inputStreamDiscoveryListener)
{
    inputStreamDiscoveryListener_ = inputStreamDiscoveryListener;

    this->dataSamples_.push_back(std::unique_ptr<rti::routing::StreamInfo>(
            new StreamInfo(SQUARE, RTI_STREAM_TYPE)));

    this->dataSamples_.push_back(std::unique_ptr<rti::routing::StreamInfo>(
            new StreamInfo(CIRCLE, RTI_STREAM_TYPE)));

    this->dataSamples_.push_back(std::unique_ptr<rti::routing::StreamInfo>(
            new StreamInfo(TRIANGLE, RTI_STREAM_TYPE)));

    /*
     * Once the RsInputDiscoveryStreamReader is initialized, we trigger an
     * event to notify that the streams are ready.
     */
    inputStreamDiscoveryListener_->on_data_available(this);
}

void RsInputDiscoveryStreamReader::take(
        std::vector<rti::routing::StreamInfo *> &stream)
{
    RTI_RS_LOG_FN(take);

    /*
     * This guard is essential since the take() and return_loan() operations
     * triggered by calling on_data_available() execute on an internal Routing
     * Service thread. The custom dispose() operation doesn't run on that
     * thread. Since the take() and return_loan() operations also need to access
     * the data_samples_ list this protection is required.
     */
    std::lock_guard<std::mutex> guard(dataSamplesMutex_);

    std::transform(
            dataSamples_.begin(),
            dataSamples_.end(),
            std::back_inserter(stream),
            [](const std::unique_ptr<rti::routing::StreamInfo> &element) {
                return element.get();
            });
}

void RsInputDiscoveryStreamReader::return_loan(
        std::vector<rti::routing::StreamInfo *> &stream)
{
    RTI_RS_LOG_FN(return_loan);

    /*
     * This guard is essential since the take() and return_loan() operations
     * triggered by calling on_data_available() execute on an internal Routing
     * Service thread. The custom dispose() operation doesn't run on that
     * thread. Since the take() and return_loan() operations also need to access
     * the data_samples_ list this protection is required.
     */
    std::lock_guard<std::mutex> guard(dataSamplesMutex_);

    /*
     * For discovery streams there will never be any outstanding return_loan().
     * Thus we can be sure that each take() will be followed by a call to
     * return_loan(), before the next take() executes.
     */
    this->dataSamples_.erase(
            dataSamples_.begin(),
            dataSamples_.begin() + stream.size());
    stream.clear();
}

void RsInputDiscoveryStreamReader::dispose(
        const rti::routing::StreamInfo &streamInfo)
{
    RTI_RS_LOG_FN(dispose);

    /*
     * This guard is essential since the take() and return_loan() operations
     * triggered by calling on_data_available() execute on an internal Routing
     * Service thread. The custom dispose() operation doesn't run on that
     * thread. Since the take() and return_loan() operations also need to access
     * the data_samples_ list this protection is required.
     */
    std::lock_guard<std::mutex> guard(dataSamplesMutex_);

    std::unique_ptr<rti::routing::StreamInfo> streamInfoDisposed(new StreamInfo(
            streamInfo.stream_name(),
            streamInfo.type_info().type_name()));
    streamInfoDisposed.get()->disposed(true);

    this->dataSamples_.push_back(std::move(streamInfoDisposed));
    inputStreamDiscoveryListener_->on_data_available(this);
}
