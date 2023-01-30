/*******************************************************************************
 (c) 2023 Copyright, Real-Time Innovations, Inc. All rights reserved.
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

#include <dds/core/ddscore.hpp>
#include <rti/cds/Service.hpp>

void service_main(std::string cfg_name, std::string cfg_file, int run_for_secs)
{
    rti::cds::ServiceProperty service_property = rti::cds::ServiceProperty()
            .cfg_file(cfg_file)
            .service_name(cfg_name)
            .application_name(
                    "CloudDiscoveryService-LibraryAPI-CXX11");
    rti::cds::Service service(service_property);

    //  Dial up the logging at service level using the public logger API
    rti::cds::Logger::instance().service_verbosity(
            static_cast<rti::config::Verbosity::type>(
                    rti::config::Verbosity::STATUS_ALL));

    service.start();

    std::cout << "ConfigName=" << cfg_name << ", "
              << "ConfigFile=" << cfg_file << std::endl;

    // Sleep for the running period
    std::this_thread::sleep_for(std::chrono::seconds(run_for_secs));

    service.stop();
}

int main(int argc, char *argv[])
{
    const std::string DEFAULT_CFG_NAME = "LibraryAPIDemo";
    const std::string DEFAULT_CFG_FILE = "CloudDiscoveryServiceConfig.xml";

    std::string cfg_name = DEFAULT_CFG_NAME;
    std::string cfg_file = DEFAULT_CFG_FILE;
    int run_for_secs = 60;

    for (int i = 1; i < argc;) {
        const std::string &param = argv[i++];

        if (param == "-cfgName" && i < argc) {
            cfg_name = argv[i++];
        } else if (param == "-cfgFile" && i < argc) {
            cfg_file = argv[i++];
        } else if (param == "-runForSecs" && i < argc) {
            run_for_secs = atoi(argv[i++]);
        } else {
            std::cout
                    << argv[0] << " [options]" << std::endl
                    << "\t-cfgName <Top level configuration name> (default: \""
                    << DEFAULT_CFG_NAME << "\")" << std::endl
                    << "\t-cfgFile <QoS configuration file name> (default: \""
                    << DEFAULT_CFG_FILE << "\")" << std::endl
                    << "\t-runForSecs <Running time of the application> "
                    << "(default: 60 secs)" << std::endl;
            return EXIT_FAILURE;
        }
    }

    // To turn on additional logging, include <rti/config/Logger.hpp> and
    // uncomment the following line:
    // rti::config::Logger::instance().verbosity(rti::config::Verbosity::STATUS_ALL);

    try {
        service_main(cfg_name, cfg_file, run_for_secs);
    } catch (const std::exception &ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in service_main: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    // Releases the memory used by the participant factory.
    // Optional at application exit
    // dds::domain::DomainParticipant::finalize_participant_factory();

    return EXIT_SUCCESS;
}