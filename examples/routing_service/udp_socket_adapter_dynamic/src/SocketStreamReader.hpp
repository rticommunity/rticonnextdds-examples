/*
 * (c) 2025 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software.  Licensee has the right to distribute object form
 * only for use with RTI products.  The Software is provided "as is", with no
 * warranty of any type, including any warranty for fitness for any purpose.
 * RTI is under no obligation to maintain or support the Software.  RTI shall
 * not be liable for any incidental or consequential damages arising out of the
 * use or inability to use the software.
 */

#ifndef SOCKETSTREAMREADER_HPP
#define SOCKETSTREAMREADER_HPP

#include <fstream>
#include <iostream>
#include <thread>
#include <queue>

#include "SocketConnection.hpp"
#include "UdpSocket.hpp"

#include <rti/routing/adapter/AdapterPlugin.hpp>
#include <rti/routing/adapter/StreamReader.hpp>

#define BUFFER_MAX_SIZE 1024
#define RECEIVE_ADDRESS_STRING "receive_address"
#define RECEIVE_PORT_STRING "receive_port"

/**
 * @brief StreamReader implementation for UDP socket input in RTI Routing Service.
 *
 * SocketStreamReader is a specific implementation of rti::routing::adapter::DynamicDataStreamReader
 * that receives data from a UDP socket and makes it available to RTI Routing Service as DynamicData samples.
 * 
 * This class manages a background thread to continuously read UDP packets from a specified address and port,
 * buffering received data for consumption by the Routing Service. It supports thread-safe queuing of incoming
 * data, loaning and returning DynamicData samples, and clean shutdown of the reading thread.
 *
 */

class SocketStreamReader : public rti::routing::adapter::DynamicDataStreamReader {
public:
    SocketStreamReader(
            SocketConnection *connection,
            const rti::routing::StreamInfo &info,
            const rti::routing::PropertySet &,
            rti::routing::adapter::StreamReaderListener *listener);

    void take(
            std::vector<dds::core::xtypes::DynamicData *> &,
            std::vector<dds::sub::SampleInfo *> &) final;

    void return_loan(
            std::vector<dds::core::xtypes::DynamicData *> &,
            std::vector<dds::sub::SampleInfo *> &) final;

    void shutdown_socket_reader_thread();

    ~SocketStreamReader();

private:
    /**
     * @brief Function used by socketreader_thread_ to read samples from the
     * socket.
     */
    void socket_reading_thread();

    SocketConnection *socket_connection_;
    rti::routing::adapter::StreamReaderListener *reader_listener_;

    std::unique_ptr<UdpSocket> socket;

    std::thread socketreader_thread_;
    bool stop_thread_;

    std::ifstream input_socket_stream_;
    std::string receive_address_;
    int receive_port_;
    char received_buffer_[BUFFER_MAX_SIZE];
    std::queue<std::vector<char>> received_buffers_;
    std::mutex buffer_mutex_;
    std::vector<char> take_buffer_;

    rti::routing::StreamInfo stream_info_;
    dds::core::xtypes::DynamicType *adapter_type_;
};

#endif
