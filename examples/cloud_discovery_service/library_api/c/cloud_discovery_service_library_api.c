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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ndds/ndds_c.h"
#include "clouddiscoveryservice/clouddiscoveryservice_service.h"

static int service_main(const char *cfgName, const char *cfgFile)
{
    struct RTI_CDS_Service *service = NULL;
    struct RTI_CDS_Property property = RTI_CDS_Property_INITIALIZER;
    DDS_Boolean ok = DDS_BOOLEAN_FALSE;
    struct DDS_Duration_t sleepPeriod = { 0, 1000000000 };

    property.cfg_file = DDS_String_dup(cfgFile);
    property.service_name = DDS_String_dup(cfgName);
    property.application_name = DDS_String_dup("CloudDiscoveryService-LibraryAPI-C");

    property.service_verbosity = RTI_CDS_LOG_VERBOSITY_EXCEPTIONS;
    property.dds_verbosity = RTI_CDS_LOG_VERBOSITY_EXCEPTIONS;


    service = RTI_CDS_Service_new(&property);
    if (service == NULL) {
        printf("Error creating the Cloud Discovery Service instance\n");
        return -1;
    }

    ok = RTI_CDS_Service_start(service);
    if (!ok) {
        printf("Error starting the Cloud Discovery Service instance\n");
        return -1;
    }

    printf("ConfigName=%s, ConfigFile=%s\n", cfgName, cfgFile);
    /* Infinite loop until the user terminates the process */
    while (1) {
        NDDS_Utility_sleep(&sleepPeriod);
    }

    ok = RTI_CDS_Service_stop(service);
    if (!ok) {
        printf("Error stopping the Cloud Discovery Service instance\n");
        return -1;
    }

    /* Sample usage of deleting the service instance for resource cleanup */
    /* RTI_CDS_Service_delete(service); */

    /* Sample usage of deleting the service property to free the strings */
    /* RTI_CDS_Property_finalize(&property); */
}

int main(int argc, char *argv[])
{
    #define DEFAULT_CFG_NAME "LibraryAPIDemo"
    #define DEFAULT_CFG_FILE "CloudDiscoveryServiceConfig.xml"

    const char *cfgName = DEFAULT_CFG_NAME;
    const char *cfgFile = DEFAULT_CFG_FILE;
    int i = 0;

    for (i = 1; i < argc;) {
        char *param = argv[i++];

        if (strcmp(param, "-cfgName") == 0 && i < argc) {
            cfgName = argv[i++];
        } else if (strcmp(param, "-cfgFile") == 0 && i < argc) {
            cfgFile = argv[i++];
        } else {
            printf("%s [options]\n"
                   "\t-cfgName <Top level configuration name> (default: \""
                   DEFAULT_CFG_NAME "\")\n"
                   "\t-cfgFile <QoS configuration file name> (default: \""
                   DEFAULT_CFG_FILE "\")\n",
                   argv[0]);
            return -1;
        }
    }

    /* Uncomment this to turn on additional logging
    NDDS_Config_Logger_set_verbosity_by_category(
            NDDS_Config_Logger_get_instance(),
            NDDS_CONFIG_LOG_CATEGORY_API,
            NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL);
    */

    return service_main(cfgName, cfgFile);
}