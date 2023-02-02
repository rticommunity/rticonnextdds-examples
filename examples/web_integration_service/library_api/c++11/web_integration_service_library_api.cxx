/*****************************************************************************/
/*         (c) Copyright, Real-Time Innovations, All rights reserved.        */
/*                                                                           */
/*         Permission to modify and use for internal purposes granted.       */
/* This software is provided "as is", without warranty, express or implied.  */
/*                                                                           */
/*****************************************************************************/

#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <map>

#include <rti/webdds/Service.hpp>

static bool shutdown_requested;

void stop_handler(int value)
{
    shutdown_requested = true;
    std::cout << "preparing to shut down...\n";
}
void setup_signal_handlers()
{
    shutdown_requested = false;
    signal(SIGINT, stop_handler);
    signal(SIGTERM, stop_handler);
}


void service_main(
        std::string cfg_name,
        std::string cfg_file,
        std::string document_root)
{
    rti::webdds::ServiceProperty service_property;

    service_property
            .cfgfile(cfg_file)
            .cfgname(cfg_name)
            .document_root(document_root);

    rti::webdds::Service service(std::move(service_property));

    std::cout << "Web Integration Service successfully created\n";

    service.start();

    std::cout << "Web Integration Service running ...\n";

    while (!shutdown_requested) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    service.stop();
}

int main(int argc, char *argv[])
{
    setup_signal_handlers();

    std::string cfg_name = "LibraryAPIDemo";
    std::string cfg_file = "WebIntegrationServiceConfig.xml";
    std::string document_root = "../../../../web_integration_service/";

    for (int i = 1; i < argc;) {
        const std::string &param = argv[i++];

        if (param == "-cfgName" && i < argc) {
            cfg_name = argv[i++];
        } else if (param == "-cfgFile" && i < argc) {
            cfg_file = argv[i++];
        } else if (param == "-documentRoot" && i < argc) {
            document_root = argv[i++];
        } else {
            std::cout
                    << argv[0] << " [options]" << std::endl
                    << "\t-cfgName <Top level configuration name> (default: \""
                    << cfg_name << "\")" << std::endl
                    << "\t-cfgFile <QoS configuration file name> (default: \""
                    << cfg_file << "\")" << std::endl
                    << "\t-documentRoot <Document root directory> (default: \""
                    << document_root << "\")" << std::endl;
            return -1;
        }
    }

    try {
        service_main(cfg_name, cfg_file, document_root);
    } catch (const std::exception &ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in service_main: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}
