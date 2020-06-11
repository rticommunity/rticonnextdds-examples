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
        const PropertySet &properties,
        StreamReaderListener *input_stream_discovery_listener)
{
    input_stream_discovery_listener_ = input_stream_discovery_listener;

    data_samples_.push_back(std::unique_ptr<rti::routing::StreamInfo>(
            new StreamInfo(SQUARE, RTI_STREAM_TYPE)));

    data_samples_.push_back(std::unique_ptr<rti::routing::StreamInfo>(
            new StreamInfo(CIRCLE, RTI_STREAM_TYPE)));

    data_samples_.push_back(std::unique_ptr<rti::routing::StreamInfo>(
            new StreamInfo(TRIANGLE, RTI_STREAM_TYPE)));

    /*
     * Once the RsInputDiscoveryStreamReader is initialized, we trigger an
     * event to notify that the streams are ready.
     */
    input_stream_discovery_listener_->on_data_available(this);
}

void RsInputDiscoveryStreamReader::take(
        std::vector<rti::routing::StreamInfo *> &stream_info)
{
    RTI_RS_LOG_FN(take);

    /*
     * This guard is essential since the take() and return_loan() operations
     * triggered by calling on_data_available() execute on an internal Routing
     * Service thread. The custom dispose() operation doesn't run on that
     * thread. Since the take() and return_loan() operations also need to access
     * the data_samples_ list this protection is required.
     */
    std::lock_guard<std::mutex> guard(data_samples_mutex_);

    std::transform(
            data_samples_.begin(),
            data_samples_.end(),
            std::back_inserter(stream_info),
            [](const std::unique_ptr<rti::routing::StreamInfo> &element) {
                return element.get();
            });
}

void RsInputDiscoveryStreamReader::return_loan(
        std::vector<rti::routing::StreamInfo *> &stream_info)
{
    RTI_RS_LOG_FN(return_loan);

    /*
     * This guard is essential since the take() and return_loan() operations
     * triggered by calling on_data_available() execute on an internal Routing
     * Service thread. The custom dispose() operation doesn't run on that
     * thread. Since the take() and return_loan() operations also need to access
     * the data_samples_ list this protection is required.
     */
    std::lock_guard<std::mutex> guard(data_samples_mutex_);

    /*
     * For discovery streams there will never be any outstanding return_loan().
     * Thus we can be sure that each take() will be followed by a call to
     * return_loan(), before the next take() executes.
     */
    data_samples_.erase(
            data_samples_.begin(),
            data_samples_.begin() + stream_info.size());
    stream_info.clear();
}

void RsInputDiscoveryStreamReader::dispose(
        const rti::routing::StreamInfo &stream_info)
{
    RTI_RS_LOG_FN(dispose);

    /*
     * This guard is essential since the take() and return_loan() operations
     * triggered by calling on_data_available() execute on an internal Routing
     * Service thread. The custom dispose() operation doesn't run on that
     * thread. Since the take() and return_loan() operations also need to access
     * the data_samples_ list this protection is required.
     */
    std::lock_guard<std::mutex> guard(data_samples_mutex_);

    std::unique_ptr<rti::routing::StreamInfo> stream_info_disposed(
            new StreamInfo(
                    stream_info.stream_name(),
                    stream_info.type_info().type_name()));
    stream_info_disposed.get()->disposed(true);

    data_samples_.push_back(std::move(stream_info_disposed));
    input_stream_discovery_listener_->on_data_available(this);
}
