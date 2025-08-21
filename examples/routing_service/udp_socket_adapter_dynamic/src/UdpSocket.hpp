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

#ifndef UDPSOCKETUTILS_HPP
#define UDPSOCKETUTILS_HPP

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <cstring>
#endif

#include <iostream>

#ifdef _WIN32
    #pragma comment(lib, "ws2_32.lib")
#endif

/**
 * @brief Utility class for UDP socket communication in the RTI Routing Service UDP Socket Adapter.
 *
 * UdpSocket provides a lightweight abstraction for UDP socket communication,
 * supporting both Windows and POSIX systems. It is designed to be used by the Routing Service
 * UDP socket adapter to send and receive raw UDP packets as part of data bridging between
 * external UDP sources and DDS.
 *
 * The class handles socket creation, binding to a specified IP address and port, and
 * ensures non-blocking operation for efficient integration with multi-threaded applications.
 * It provides methods for receiving data from any UDP client and for sending data to a
 * specified destination address and port. 
 */

class UdpSocket {
public:
    UdpSocket(const char* ip, int port);
    ~UdpSocket();
    void receive_data(
            char* received_buffer,
            int* received_bytes,
            int size_of_original_buffer);

    int send_data(
            char* tx_buffer, 
            int tx_length, 
            const char* destAddr,
			int destPort);        

private:
#ifdef _WIN32
    SOCKET sockfd;
#else
    int sockfd;
#endif
    struct sockaddr_in server_addr, client_addr;

    void init_socket();
    void bind_socket(const char* ip, int port);
};

#endif
