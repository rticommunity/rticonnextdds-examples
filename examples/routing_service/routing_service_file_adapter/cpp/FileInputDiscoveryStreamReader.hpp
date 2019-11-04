/*****************************************************************************/
/*         (c) Copyright, Real-Time Innovations, All rights reserved.        */
/*                                                                           */
/*         Permission to modify and use for internal purposes granted.       */
/* This software is provided "as is", without warranty, express or implied.  */
/*                                                                           */
/*****************************************************************************/
#ifndef FILEDISCOVERYSTREAMREADER_HPP
#define FILEDISCOVERYSTREAMREADER_HPP

#include <fstream>

#include <rti/routing/adapter/AdapterPlugin.hpp>
#include <rti/routing/adapter/DiscoveryStreamReader.hpp>

namespace rti { namespace community { namespace examples {

/** 
 * This class implements a DiscoveryStreamReader, a special kind of StreamReader
 * that provide discovery information about the available streams and their
 * types.
 */

class FileInputDiscoveryStreamReader : 
        public rti::routing::adapter::DiscoveryStreamReader {
public:
    FileInputDiscoveryStreamReader(
            const rti::routing::PropertySet &, 
            rti::routing::adapter::StreamReaderListener *input_stream_discovery_listener);

    void take(std::vector<rti::routing::StreamInfo*>&) final;

    void return_loan(std::vector<rti::routing::StreamInfo*>&) final;

    void dispose(const rti::routing::StreamInfo &stream_info);

    bool fexists(const std::string filename);

private:

    static const std::string SQUARE_FILE_NAME;
    static const std::string CIRCLE_FILE_NAME;
    static const std::string TRIANGLE_FILE_NAME;

    std::mutex data_samples_mutex_;
    std::list<rti::routing::StreamInfo *> data_samples_;
    rti::routing::adapter::StreamReaderListener *input_stream_discovery_listener_;
};

}  // namespace examples
}  // namespace community
}  // namespace rti

#endif
