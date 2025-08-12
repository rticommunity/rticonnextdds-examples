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

#include <dds/dds.hpp>
#include <rti/rti.hpp>
#include <rti/routing/Logger.hpp>
#include "SocketStreamWriter.hpp"
#include "SocketStreamReader.hpp" //use ShapeType from here 
#include <rti/core/Exception.hpp>

using namespace dds::core::xtypes;
using namespace rti::routing;
using namespace rti::routing::adapter;

SocketStreamWriter::SocketStreamWriter(
        SocketConnection *connection,
        const StreamInfo &info,
        const PropertySet &properties
        )
        : stream_info_(info.stream_name(), info.type_info().type_name())
{

    socket_connection_ = connection;
   
    adapter_type_ =
            static_cast<DynamicType *>(info.type_info().type_representation());


	// Parse the properties provided in the xml configuration file
	for (const auto &property : properties) {
		if (property.first == SEND_ADDRESS_STRING) {
			send_address_ = property.second;
		}
		else if (property.first == SEND_PORT_STRING) {
			send_port_ = std::stoi(property.second);
		}
		else if (property.first == DEST_ADDRESS_STRING)
		{
			dest_address_ = property.second;
		}
		else if (property.first == DEST_PORT_STRING)
		{
			dest_port_ = std::stoi(property.second);
		}		
	}

	socket = std::unique_ptr<UdpSocket>(new UdpSocket(
		send_address_.c_str(),
		send_port_));
}

int SocketStreamWriter::write(
        const std::vector<dds::core::xtypes::DynamicData *> &samples,
        const std::vector<dds::sub::SampleInfo *> &infos)        
{
    size_t len = 0;
    for (const auto sample : samples) {
        std::vector<char> buffer;
        rti::core::xtypes::to_cdr_buffer(buffer, *sample);
	 // Send the serialized data
        len = socket->send_data(
            buffer.data(),
            buffer.size(),
            dest_address_.c_str(),
            dest_port_);
    }
	
    return len;
}

int SocketStreamWriter::write(
        const std::vector<dds::core::xtypes::DynamicData *> &samples,
        const std::vector<dds::sub::SampleInfo *> &infos,
        const SelectorState &selector_state)
{
    int len;
    len = write(samples, infos);
    return len;
}

void SocketStreamWriter::return_loan(
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

SocketStreamWriter::~SocketStreamWriter()
{
}
