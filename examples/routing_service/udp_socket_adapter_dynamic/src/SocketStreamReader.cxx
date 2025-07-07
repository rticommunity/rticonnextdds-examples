/*
 * (c) 2024 Copyright, Real-Time Innovations, Inc.  All rights reserved.
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
#include <rti/topic/cdr/Serialization.hpp>

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
        }
    }

    // If any of the mandatory properties is not specified, throw exception
    if (receive_address_.size() == 0 || receive_port_ == 0) {
        throw dds::core::IllegalOperationError(
                "You must set receive_address and receive_port "
                "in the RsSocketAdapter.xml file");
    }

    // Create the UDP socket to receive data
    socket = std::unique_ptr<UdpSocket>(
            new UdpSocket(receive_address_.c_str(), receive_port_));

    // Start the receive thread for UDP data
    socketreader_thread_ =
            std::thread(&SocketStreamReader::socket_reading_thread, this);
}

/**
 * This is the Routing Service take(). It's called when the
 * socket_receive_thread calls on_data_available()
 */
void SocketStreamReader::take(
        std::vector<dds::core::xtypes::DynamicData *> &samples,
        std::vector<dds::sub::SampleInfo *> &infos)
{
    dds::core::xtypes::DynamicData deserialized_sample(*adapter_type_);
    std::vector<char> received_buffer = std::vector<char>(received_buffer_, received_buffer_ + received_bytes_);
    rti::core::xtypes::from_cdr_buffer(deserialized_sample, received_buffer);
    std::cout << deserialized_sample << std::endl;

    samples.resize(1);
    infos.resize(1);
    
    std::unique_ptr<DynamicData> sample(new DynamicData(*adapter_type_));
    *sample = deserialized_sample;
    samples[0] = sample.release();

    return;
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
