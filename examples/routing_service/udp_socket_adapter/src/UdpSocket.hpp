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

class UdpSocket {
public:
    UdpSocket(const char* ip, int port);
    ~UdpSocket();
    void receive_data(
            char* received_buffer,
            int* received_bytes,
            int size_of_original_buffer);

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
