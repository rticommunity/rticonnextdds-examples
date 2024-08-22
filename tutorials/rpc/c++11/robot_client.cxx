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

#include <iostream>
#include "robot.hpp"

int main(int argc, char *argv[])
{
    dds::domain::DomainParticipant participant(0);

    dds::rpc::ClientParams client_params(participant);
    client_params.service_name("Example RobotControl Service");

    RobotControlClient client(client_params);

    client.wait_for_service();

    std::cout << "Calling walk_to..." << std::endl;
    std::future<Coordinates> result =
            client.walk_to_async(Coordinates(5, 10), 30.0);
    std::cout << result.get() << std::endl;

    return 0;
}
