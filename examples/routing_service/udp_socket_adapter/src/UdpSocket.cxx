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

#include "UdpSocket.hpp"

#include <rti/core/Exception.hpp>


UdpSocket::UdpSocket(const char *ip, int port)
{
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw dds::core::IllegalOperationError("WSAStartup failed");
    }
#endif

    // Socket initialization
    init_socket();
    memset(&server_addr, 0, sizeof(server_addr));

    // Using non-blocking sockets for easier thread management
#ifdef _WIN32
    unsigned long nonBlocking = 1;
    if (ioctlsocket(sockfd, FIONBIO, &nonBlocking) != 0) {
        std::cerr << "Error setting socket to non-blocking\n";
        closesocket(sockfd);
        WSACleanup();
        throw dds::core::IllegalOperationError("ioctlsocket failed");
    }
#else
    fcntl(sockfd, F_SETFL, O_NONBLOCK);
#endif

    // Bind the socket
    bind_socket(ip, port);
}

UdpSocket::~UdpSocket()
{
#ifdef _WIN32
    closesocket(sockfd);
    WSACleanup();
#else
    close(sockfd);
#endif
}

void UdpSocket::init_socket()
{
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        throw dds::core::IllegalOperationError("Socket creation failed");
    }
}

void UdpSocket::bind_socket(const char* ip, int port)
{
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &(server_addr.sin_addr));
    server_addr.sin_port = htons(port);

    if (bind(sockfd,
             (const struct sockaddr *) &server_addr,
             sizeof(server_addr))
        == -1) {
        throw dds::core::IllegalOperationError("Bind failed");
    }
}

void UdpSocket::receive_data(
        char *received_buffer,
        int *received_bytes,
        int size_of_original_buffer)
{
    socklen_t len = sizeof(server_addr);

    socklen_t client_addr_len = sizeof(client_addr);

    /** Receive data.Since it's non-blocking, it will return right away most
     * of the times
     */
    *received_bytes = recvfrom(
            sockfd,
            received_buffer,
            size_of_original_buffer,
            0,
            (struct sockaddr *) &client_addr,
            &client_addr_len);

    return;
}