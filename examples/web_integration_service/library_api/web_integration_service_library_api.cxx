/*****************************************************************************/
/*         (c) Copyright, Real-Time Innovations, All rights reserved.        */
/*                                                                           */
/*         Permission to modify and use for internal purposes granted.       */
/* This software is provided "as is", without warranty, express or implied.  */
/*                                                                           */
/*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "ndds/ndds_c.h"
#include "rti/webdds/Service.hpp"

static RTIBool shutdown_requested;

void stop_handler(int value)
{
    shutdown_requested = RTI_TRUE;
    printf("preparing to shut down...\n");
}
void setup_signal_handlers()
{
    shutdown_requested = RTI_FALSE;
    signal(SIGINT, stop_handler);
    signal(SIGTERM, stop_handler);
}

#ifdef RTI_IOS
extern int web_integration_service_main(const char * cfg_file, const char * cfg_name)
#else
static int web_integration_service_main(const char * cfg_file, const char * cfg_name)
#endif
{
    struct DDS_Duration_t wait_period = {4,0};

    rti::webdds::ServiceProperty service_property;

    service_property.cfgfile(cfg_file).cfgname(cfg_name);

    rti::webdds::Service service(std::move(service_property));

    setup_signal_handlers();

    printf("Web Integration Service successfully created\n");

    service.start();

    printf("Web Integration Service running ...\n");

#ifndef RTI_IOS
    /* wait to be stopped */
    while(!shutdown_requested) {
        NDDS_Utility_sleep(&wait_period);
    }
#else
    /* start web_integration_service_main() in a thread. */
#endif

    

    service.stop();

    if (DDS_DomainParticipantFactory_finalize_instance() != DDS_RETCODE_OK) {
        perror ("Error: failed to finalize DomainParticipantFactory\n");
        return -1;
    }

    return 1;
}

#if !(defined(RTI_VXWORKS) && !defined(__RTP__)) && !defined(RTI_PSOS) && !defined(RTI_IOS)
int main(int argc, char *argv[])
{
    char * cfg_file = NULL;
    char * cfg_name = NULL;
    if (argc >= 3) {
        cfg_file = argv[1];
        cfg_name = argv[2];
    }

    return web_integration_service_main(cfg_file, cfg_name);
}
#endif
