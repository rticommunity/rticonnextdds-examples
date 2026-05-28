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

#ifndef SOCKETSTREAMWRITER_HPP
#define SOCKETSTREAMWRITER_HPP

#include <fstream>
#include <iostream>
#include <thread>
#include <cstring>

#include "SocketConnection.hpp"
#include "UdpSocket.hpp"

#include <rti/routing/adapter/AdapterPlugin.hpp>
#include <rti/routing/adapter/StreamReader.hpp>

#define BUFFER_MAX_SIZE 1024
#define SEND_ADDRESS_STRING "send_address"
#define SEND_PORT_STRING "send_port"
#define DEST_ADDRESS_STRING "dest_address"
#define DEST_PORT_STRING "dest_port"

/**
 * @brief StreamWriter implementation for UDP socket output in RTI Routing Service.
 *
 * SocketStreamWriter is a specific implementation of rti::routing::adapter::DynamicDataStreamWriter
 * that sends data to a UDP socket, making it available for external consumers outside DDS.
 *
 * This class is responsible for serializing DynamicData samples received from Routing Service
 * and transmitting them as UDP packets to a specified destination address and port.
 * It manages socket creation, serialization buffers, and the configuration of destination
 * parameters via properties.
 *
 */

class SocketStreamWriter : public rti::routing::adapter::DynamicDataStreamWriter {
public:
    explicit SocketStreamWriter(
            SocketConnection *connection,
            const rti::routing::StreamInfo &info,
            const rti::routing::PropertySet &
            );

    virtual int write(
            const std::vector<dds::core::xtypes::DynamicData *> &,
            const std::vector<dds::sub::SampleInfo *> &) final;

    ~SocketStreamWriter();
	

private:

    SocketConnection *socket_connection_;
    std::vector<char> serialization_buffer_;
    std::unique_ptr<UdpSocket> socket;

    int send_port_;
    int dest_port_;

    std::string send_address_;
    std::string dest_address_;
    rti::routing::StreamInfo stream_info_;
    dds::core::xtypes::DynamicType *adapter_type_;
};

#endif
