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

#ifndef RS_INPUT_DISCOVERY_STREAM_READER_HPP_
#define RS_INPUT_DISCOVERY_STREAM_READER_HPP_


#include <rti/routing/adapter/StreamReaderListener.hpp>
#include <rti/routing/adapter/DiscoveryStreamReader.hpp>
#include <rti/routing/PropertySet.hpp>
#include <mutex>

/*
 * This class implements a DiscoveryStreamReader, a special kind of StreamReader
 * that provide discovery information about the available streams and their
 * types.
 */

class RsInputDiscoveryStreamReader
        : public rti::routing::adapter::DiscoveryStreamReader {
public:
    RsInputDiscoveryStreamReader(
            const rti::routing::PropertySet &properties,
            rti::routing::adapter::StreamReaderListener
                    *input_stream_discovery_listener);

    virtual ~RsInputDiscoveryStreamReader() = default;

    void take(std::vector<rti::routing::StreamInfo *> &stream_info) final;

    void return_loan(
            std::vector<rti::routing::StreamInfo *> &stream_info) final;

    /*
     * @brief Custom operation defined to indicate disposing off an <input>
     * when the RsStreamReader has finished reading from the database.
     * The RsInputDiscoveryStreamReader will then create a new
     * discovery sample indicating that the stream has been disposed.
     * This will cause the Routing Service to start tearing down the Routes
     * associated with <input> having the corresponding <registered_type_name>
     * and <stream_name>.
     *
     * @param stream_info \b in. Reference to a StreamInfo object which should
     * be used when creating a new StreamInfo sample with disposed set to true
     */
    void dispose(const rti::routing::StreamInfo &stream_info);

private:
    std::mutex data_samples_mutex_;
    std::vector<std::unique_ptr<rti::routing::StreamInfo>> data_samples_;
    rti::routing::adapter::StreamReaderListener *input_stream_discovery_listener_;
};

#endif /* RS_INPUT_DISCOVERY_STREAM_READER_HPP_ */
