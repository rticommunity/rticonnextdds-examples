/*
 * (c) 2019 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software.  Licensee has the right to distribute object form
 * only for use with RTI products.  The Software is provided "as is", with no
 * warranty of any type, including any warranty for fitness for any purpose.
 * RTI is under no obligation to maintain or support the Software.  RTI shall
 * not be liable for any incidental or consequential damages arising out of the
 * use or inability to use the software.
 */

#include "FileInputDiscoveryStreamReader.hpp"

using namespace rti::routing;
using namespace rti::routing::adapter;
using namespace rti::community::examples;

const std::string FileInputDiscoveryStreamReader::SQUARE_FILE_NAME =
        "Input_Square.csv";
const std::string FileInputDiscoveryStreamReader::CIRCLE_FILE_NAME =
        "Input_Circle.csv";
const std::string FileInputDiscoveryStreamReader::TRIANGLE_FILE_NAME =
        "Input_Triangle.csv";

bool FileInputDiscoveryStreamReader::fexists(const std::string filename)
{
    std::ifstream input_file;
    input_file.open(filename);
    return input_file.is_open();
}

FileInputDiscoveryStreamReader::FileInputDiscoveryStreamReader(
        const PropertySet &,
        StreamReaderListener *input_stream_discovery_listener)
{
    input_stream_discovery_listener_ = input_stream_discovery_listener;

    /**
     * In our example, we provide statically the stream information available.
     * We do not have a mechanism demonstrating how to perform discovery after
     * startup. However, as an idea you can have a thread monitoring the file
     * system and updating the list of StreamInfo samples and calling
     * input_stream_discovery_listener_->on_data_available(this); to notify that
     * new files have been discovered.
     */
    if (fexists(SQUARE_FILE_NAME)) {
        this->data_samples_.push_back(std::unique_ptr<rti::routing::StreamInfo>(
                new StreamInfo("Square", "ShapeType")));
    }

    if (fexists(CIRCLE_FILE_NAME)) {
        this->data_samples_.push_back(std::unique_ptr<rti::routing::StreamInfo>(
                new StreamInfo("Circle", "ShapeType")));
    }

    if (fexists(TRIANGLE_FILE_NAME)) {
        this->data_samples_.push_back(std::unique_ptr<rti::routing::StreamInfo>(
                new StreamInfo("Triangle", "ShapeType")));
    }

    /**
     * Once the FileInputDiscoveryStreamReader is initialized, we trigger an
     * event to notify that the streams are ready.
     */
    input_stream_discovery_listener_->on_data_available(this);
}

void FileInputDiscoveryStreamReader::dispose(
        const rti::routing::StreamInfo &stream_info)
{
    /**
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

    this->data_samples_.push_back(std::move(stream_info_disposed));
    input_stream_discovery_listener_->on_data_available(this);
}

void FileInputDiscoveryStreamReader::take(
        std::vector<rti::routing::StreamInfo *> &stream)
{
    /**
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
            std::back_inserter(stream),
            [](const std::unique_ptr<rti::routing::StreamInfo> &element) {
                return element.get();
            });
}

void FileInputDiscoveryStreamReader::return_loan(
        std::vector<rti::routing::StreamInfo *> &stream)
{
    /**
     * This guard is essential since the take() and return_loan() operations
     * triggered by calling on_data_available() execute on an internal Routing
     * Service thread. The custom dispose() operation doesn't run on that
     * thread. Since the take() and return_loan() operations also need to access
     * the data_samples_ list this protection is required.
     */
    std::lock_guard<std::mutex> guard(data_samples_mutex_);

    /**
     * For discovery streams there will never be any outstanding return_loan().
     * Thus we can be sure that each take() will be followed by a call to
     * return_loan(), before the next take() executes.
     */
    this->data_samples_.erase(
            data_samples_.begin(),
            data_samples_.begin() + stream.size());
    stream.clear();
}
