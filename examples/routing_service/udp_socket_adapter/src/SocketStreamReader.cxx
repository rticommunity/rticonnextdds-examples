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

#include <algorithm>
#include <cctype>
#include <sstream>
#include <thread>
#include <chrono>

#include <iostream>
#include <cstring>
#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

#include "SocketStreamReader.hpp"
#include <rti/core/Exception.hpp>

using namespace dds::core::xtypes;
using namespace rti::routing;
using namespace rti::routing::adapter;

void SocketStreamReader::socket_reading_thread()
{
    while (!stop_thread_) {
        /**
         * Essential to protect against concurrent data access to
         * buffer_ from the take() methods running on a different
         * Routing Service thread.
         */
        std::unique_lock<std::mutex> lock(buffer_mutex_);
        socket->receive_data(
                received_buffer_,
                &received_bytes_,
                BUFFER_MAX_SIZE);
        lock.unlock();

        // Most likely received nothing or there was an error
        // Not doing any error handling here
        if (received_bytes_ <= 0) {
            // Sleep for a small period of time to avoid busy waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        /**
         * Here we notify Routing Service, that there is data available
         * on the StreamReader, triggering a call to take().
         */
        reader_listener_->on_data_available(this);
    }

    socket_connection_->dispose_discovery_stream(stream_info_);
}

SocketStreamReader::SocketStreamReader(
        SocketConnection *connection,
        const StreamInfo &info,
        const PropertySet &properties,
        StreamReaderListener *listener)
        : stop_thread_(false),
          stream_info_(info.stream_name(), info.type_info().type_name())
{
    socket_connection_ = connection;
    reader_listener_ = listener;
    adapter_type_ =
            static_cast<DynamicType *>(info.type_info().type_representation());

    // Parse the properties provided in the xml configuration file
    for (const auto &property : properties) {
        if (property.first == RECEIVE_ADDRESS_STRING) {
            receive_address_ = property.second;
        } else if (property.first == RECEIVE_PORT_STRING) {
            receive_port_ = std::stoi(property.second);
        } else if (property.first == SHAPE_COLOR_STRING) {
            shape_color_ = property.second;
        }
    }

    if (receive_address_.size() == 0
            || receive_port_ == 0
            || shape_color_.size() == 0) {
        throw dds::core::IllegalOperationError(
                "You must set receive_address, receive_port and" \
                " shape_color in the RsSocketAdapter.xml file");
    }

    socket = std::unique_ptr<UdpSocket>(new UdpSocket(
            receive_address_.c_str(),
            receive_port_));

    socketreader_thread_ =
            std::thread(&SocketStreamReader::socket_reading_thread, this);
}

void SocketStreamReader::take(
        std::vector<dds::core::xtypes::DynamicData *> &samples,
        std::vector<dds::sub::SampleInfo *> &infos)
{
    if (stream_info_.stream_name() == "Square") {
        /**
         * This protection is required since take() executes on a different
         * Routing Service thread.
         */
        std::unique_lock<std::mutex> lock(buffer_mutex_);
        ShapeType* shape = reinterpret_cast<ShapeType*>(received_buffer_);
        lock.unlock();

        /**
         * Note that we read one packet at a time from socket_reading_thread()
         */
        samples.resize(1);
        infos.resize(1);

        std::unique_ptr<DynamicData> sample(new DynamicData(*adapter_type_));

        /**
         * This is the hardcoded type information about ShapeType.
         * You are advised to change this as per your type definition
         */
        sample->value("x", shape->x);
        sample->value("y", shape->y);
        sample->value("shapesize", shape->shapesize);
        // Color is retrieved from the XML configuration
        sample->value("color", shape_color_);

        samples[0] = sample.release();

    }

    return;
}

void SocketStreamReader::print_shape(ShapeType shape) {

    std::cout << "x: " << shape.x << std::endl;
    std::cout << "y: " << shape.y << std::endl;
    std::cout << "shapesize: " << shape.shapesize << std::endl;
};

void SocketStreamReader::take(
        std::vector<dds::core::xtypes::DynamicData *> &samples,
        std::vector<dds::sub::SampleInfo *> &infos,
        const SelectorState &selector_state)
{
    take(samples, infos);
}

void SocketStreamReader::return_loan(
        std::vector<dds::core::xtypes::DynamicData *> &samples,
        std::vector<dds::sub::SampleInfo *> &infos)
{
    for (int i = 0; i < samples.size(); ++i) {
        delete samples[i];
        delete infos[i];
    }
    samples.clear();
    infos.clear();
}

void SocketStreamReader::shutdown_socket_reader_thread()
{
    stop_thread_ = true;
    socketreader_thread_.join();
}

SocketStreamReader::~SocketStreamReader()
{
}
