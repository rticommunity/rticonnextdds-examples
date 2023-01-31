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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ndds/ndds_c.h"
#include "clouddiscoveryservice/clouddiscoveryservice_service.h"

int service_shutdown(
        struct RTI_CDS_Service *service,
        struct RTI_CDS_Property *property)
{
    DDS_Boolean ok = DDS_BOOLEAN_FALSE;

    if (service != NULL) {
        ok = RTI_CDS_Service_stop(service);
        if (!ok) {
            printf("Error stopping the Cloud Discovery Service instance\n");
            return EXIT_FAILURE;
        }

        /* Deleting the service instance for resource cleanup */
        RTI_CDS_Service_delete(service);
    }

    if (property != NULL) {
        /* Deleting the service property to free the strings */
        RTI_CDS_Property_finalize(property);
    }

    return EXIT_SUCCESS;
}

int service_main(const char *cfgName, const char *cfgFile, int runForSecs)
{
    struct RTI_CDS_Service *service = NULL;
    struct RTI_CDS_Property property = RTI_CDS_Property_INITIALIZER;
    DDS_Boolean ok = DDS_BOOLEAN_FALSE;
    struct DDS_Duration_t runningPeriod = { 0, 0 };

    runningPeriod.sec = runForSecs;

    property.cfg_file = DDS_String_dup(cfgFile);
    property.service_name = DDS_String_dup(cfgName);
    property.application_name =
            DDS_String_dup("CloudDiscoveryService-LibraryAPI-C");

    /* Dial up the logging at service level */
    property.service_verbosity = RTI_CDS_LOG_VERBOSITY_ALL;
    property.dds_verbosity = RTI_CDS_LOG_VERBOSITY_EXCEPTIONS;


    service = RTI_CDS_Service_new(&property);
    if (service == NULL) {
        printf("Error creating the Cloud Discovery Service instance\n");
        service_shutdown(service, &property);
        return EXIT_FAILURE;
    }

    ok = RTI_CDS_Service_start(service);
    if (!ok) {
        printf("Error starting the Cloud Discovery Service instance\n");
        service_shutdown(service, &property);
        return EXIT_FAILURE;
    }

    printf("ConfigName=%s, ConfigFile=%s\n", cfgName, cfgFile);

    /* Sleep for the running period */
    NDDS_Utility_sleep(&runningPeriod);

    return service_shutdown(service, &property);
}

int main(int argc, char *argv[])
{
    const char *cfgName = "LibraryAPIDemo";                  /* default */
    const char *cfgFile = "CloudDiscoveryServiceConfig.xml"; /* default */
    int i = 0;
    int runForSecs = 60;
    int retVal = EXIT_SUCCESS;

    for (i = 1; i < argc;) {
        char *param = argv[i++];

        if (strcmp(param, "-cfgName") == 0 && i < argc) {
            cfgName = argv[i++];
        } else if (strcmp(param, "-cfgFile") == 0 && i < argc) {
            cfgFile = argv[i++];
        } else if (strcmp(param, "-runForSecs") == 0 && i < argc) {
            runForSecs = atoi(argv[i++]);
        } else {
            printf("%s [options]\n"
                   "\t-cfgName <Top level configuration name> (default: "
                   "\"%s\")\n"
                   "\t-cfgFile <QoS configuration file name> (default: "
                   "\"%s\")\n"
                   "\t-runForSecs <Running time of the application> (default: "
                   "60 secs)\n",
                   argv[0],
                   cfgName,
                   cfgFile);
            return EXIT_FAILURE;
        }
    }

    /* Uncomment this to turn on additional logging
    NDDS_Config_Logger_set_verbosity_by_category(
            NDDS_Config_Logger_get_instance(),
            NDDS_CONFIG_LOG_CATEGORY_API,
            NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL);
    */

    retVal = service_main(cfgName, cfgFile, runForSecs);

    /*
     * RTI Data Distribution Service provides the finalize_instance() method on
     * domain participant factory for users who want to release memory used
     * by the participant factory. Uncomment the following block of code for
     * clean destruction of the singleton.
     */
    /*
    DDS_ReturnCode_t retcode;
    retcode = DDS_DomainParticipantFactory_finalize_instance();
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "finalize_instance error %d\n", retcode);
        retVal = EXIT_FAILURE;
    }
    */

    return retVal;
}