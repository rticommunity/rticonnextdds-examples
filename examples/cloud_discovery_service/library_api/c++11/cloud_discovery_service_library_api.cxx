/*******************************************************************************
 (c) 2005-2023 Copyright, Real-Time Innovations, Inc. All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software. Licensee has the right to distribute object form only
 for use with RTI products. The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software. RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/

#include <iostream>
#include <thread>
#include <chrono>
#include <map>

#include <rti/cds/Service.hpp>

void service_main(std::string cfg_name, std::string cfg_file)
{
    rti::cds::Service service(
            rti::cds::ServiceProperty()
                    .cfg_file(cfg_file)
                    .service_name(cfg_name)
                    .application_name("CloudDiscoveryService-LibraryAPI-CXX11"));

    //  Dial up the logging at service level using the public logger API
    rti::cds::Logger::instance().service_verbosity(
            static_cast<rti::config::Verbosity::type>(
                    rti::config::Verbosity::STATUS_ALL));

    service.start();

    std::cout << "ConfigName=" << cfg_name << ", "
              << "ConfigFile=" << cfg_file << std::endl;
    // Infinite loop until the user terminates the process
    while (1) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    service.stop();
}

int main(int argc, char *argv[])
{
    const std::string DEFAULT_CFG_NAME = "LibraryAPIDemo";
    const std::string DEFAULT_CFG_FILE = "CloudDiscoveryServiceConfig.xml";

    std::string cfg_name = DEFAULT_CFG_NAME;
    std::string cfg_file = DEFAULT_CFG_FILE;

    for (int i = 1; i < argc;) {
        const std::string &param = argv[i++];

        if (param == "-cfgName" && i < argc) {
            cfg_name = argv[i++];
        } else if (param == "-cfgFile" && i < argc) {
            cfg_file = argv[i++];
        } else {
            std::cout << argv[0]
                      << " [options]" << std::endl
                      << "\t-cfgName <Top level configuration name> (default: \""
                      << DEFAULT_CFG_NAME << "\")" << std::endl
                      << "\t-cfgFile <QoS configuration file name> (default: \""
                      << DEFAULT_CFG_FILE << "\")" << std::endl;
            return -1;
        }
    }

    // To turn on additional logging, include <rti/config/Logger.hpp> and
    // uncomment the following line:
    // rti::config::Logger::instance().verbosity(rti::config::Verbosity::STATUS_ALL);

    try {
        service_main(cfg_name, cfg_file);
    } catch (const std::exception &ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in service_main: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}