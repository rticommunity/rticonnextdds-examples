/*
 * (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software.  Licensee has the right to distribute object form
 * only for use with RTI products.  The Software is provided "as is", with no
 * warranty of any type, including any warranty for fitness for any purpose.
 * RTI is under no obligation to maintain or support the Software.  RTI shall
 * not be liable for any incidental or consequential damages arising out of the
 * use or inability to use the software.
 */

#include "RsAdapterPlugin.hpp"
#include "RsLog.hpp"
#include <dds/core/ddscore.hpp>
#include <istream>
#include <rti/routing/PropertySet.hpp>
#include <rti/routing/RoutingService.hpp>
#include <rti/routing/ServiceProperty.hpp>
#include <sys/stat.h>

#define RTI_RS_LOG_ARGS "rsmain"

HANDLE g_hTerminateEvent;

BOOL WINAPI RsCtrlHandler(DWORD dwCtrlType)
{
    if (dwCtrlType == CTRL_C_EVENT || dwCtrlType == CTRL_BREAK_EVENT
        || dwCtrlType == CTRL_CLOSE_EVENT) {
        std::cout << "Terminating" << std::endl;
        SetEvent(g_hTerminateEvent);

        return TRUE;
    }
    return FALSE;
}

inline bool exists(const std::string &filename)
{
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

using namespace rti::routing;
int main(int argc, char *argv[])
{
    if (argc != 3) {
        RTI_RS_ERROR("Usage: rsmain.exe <xml_file> <routing_service_name>");
    } else if (!exists(argv[1])) {
        RTI_RS_ERROR("Error: XML file does not exist");
    } else {
        PropertySet properties;
        ServiceProperty serviceProperty;

        g_hTerminateEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
        SetConsoleCtrlHandler(RsCtrlHandler, TRUE);

        try {
            serviceProperty.cfg_file(argv[1]);
            serviceProperty.service_name(argv[2]);
            RoutingService service(serviceProperty);
            RsAdapterPlugin *myRsAdapterPlugin =
                    new RsAdapterPlugin(properties);
            /* plugin object is owned by the RS once attach_adapter_plugin()
             * is called. Do not delete this plugin object. */
            service.attach_adapter_plugin(myRsAdapterPlugin, "myMsqlAdapter");
            service.start();

            std::cout << "Enter Ctrl-C to quit" << std::endl;

            /* wait for a Control-C event */
            if (g_hTerminateEvent) {
                WaitForSingleObject(g_hTerminateEvent, INFINITE);
                CloseHandle(g_hTerminateEvent);
            }

            service.stop();
            service.finalize_globals();
        } catch (dds::core::Exception &ex) {
            std::cerr << ex.what() << std::endl;
            return EXIT_FAILURE;
        } catch (std::runtime_error &ex) {
            std::cerr << ex.what() << std::endl;
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
    ;
}
