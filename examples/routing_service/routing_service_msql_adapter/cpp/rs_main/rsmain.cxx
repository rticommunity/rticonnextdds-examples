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

HANDLE g_h_terminate_event;

BOOL WINAPI rs_ctrl_handler(DWORD dw_ctrl_type)
{
    if (dw_ctrl_type == CTRL_C_EVENT || dw_ctrl_type == CTRL_BREAK_EVENT
        || dw_ctrl_type == CTRL_CLOSE_EVENT) {
        std::cout << "Terminating" << std::endl;
        SetEvent(g_h_terminate_event);

        return TRUE;
    }
    return FALSE;
}

inline bool exists(const std::string &file_name)
{
    struct stat buffer;
    return (stat(file_name.c_str(), &buffer) == 0);
}

void verify_args(int argc, char *argv[])
{
    if (argc != 3) {
        throw dds::core::Error(
                "Usage: rsmain.exe <xml_file> <routing_service_name>");
    } else if (!exists(argv[1])) {
        throw dds::core::Error("Error: XML file does not exist");
    }
}

using namespace rti::routing;
int main(int argc, char *argv[])
{
    try {
        verify_args(argc, argv);

        PropertySet properties;
        ServiceProperty service_property;

        g_h_terminate_event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
        SetConsoleCtrlHandler(rs_ctrl_handler, TRUE);

        service_property.cfg_file(argv[1]);
        service_property.service_name(argv[2]);
        RoutingService service(service_property);
        RsAdapterPlugin *rs_adapter_plugin = new RsAdapterPlugin(properties);
        /* plugin object is owned by the RS once attach_adapter_plugin()
         * is called. Do not delete this plugin object. */
        service.attach_adapter_plugin(rs_adapter_plugin, "myMsqlAdapter");
        service.start();

        std::cout << "Enter Ctrl-C to quit" << std::endl;

        /* wait for a Control-C event */
        if (g_h_terminate_event) {
            WaitForSingleObject(g_h_terminate_event, INFINITE);
            CloseHandle(g_h_terminate_event);
        }

        service.stop();
        service.finalize_globals();
    } catch (dds::core::Error &ex) {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
