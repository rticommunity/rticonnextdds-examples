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

class RobotControlExample : public RobotControl {
public:
    Coordinates walk_to(const ::Coordinates &destination, float speed) override
    {
        if (speed <= 0.0) {
            return position_;
        }

        speed_ = speed;

        std::cout << "Robot walking at " << speed_ << "mph to " << destination
                  << std::endl;

        // Simulate walking to the destination
        std::this_thread::sleep_for(std::chrono::milliseconds(
                static_cast<int>(10000 - 100 * speed_)));

        std::cout << "Robot arrived at " << destination << std::endl;

        speed_ = 0.0;
        position_ = destination;
        return destination;
    }

    float get_speed() override
    {
        return speed_;
    }

private:
    Coordinates position_;
    float speed_ { 0.0 };
};

int main(int argc, char *argv[])
{
    dds::domain::DomainParticipant participant(0);

    dds::rpc::ServerParams server_params;
    server_params.extensions().thread_pool_size(4);
    dds::rpc::Server server(server_params);

    dds::rpc::ServiceParams params(participant);
    params.service_name("Example RobotControl Service");
    auto service_impl = std::make_shared<RobotControlExample>();
    RobotControlService service(service_impl, server, params);

    std::cout << "RobotControlService running... " << std::endl;
    server.run();

    return 0;
}
