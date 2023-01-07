/*******************************************************************************
 (c) 2005-2023 Copyright, Real-Time Innovations, Inc.  All rights reserved.
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
#include "persistence/persistence_service.h"

static int service_main(int domainId, int isPersistent)
{
    struct RTI_PersistenceServiceProperty property =
            RTI_PersistenceServiceProperty_INITIALIZER;
    struct RTI_PersistenceService *service = NULL;
    DDS_Boolean ok = DDS_BOOLEAN_FALSE;
    struct DDS_Duration_t sleepPeriod = {0, 1000000000};

    property.cfg_file = "PersistenceServiceConfig.xml";
    property.cfg_name = isPersistent ? "defaultPersistent" : "defaultTransient";
    property.domain_id = domainId;
    property.application_name = (char *) "PersistenceServiceLibraryAPI";

    property.service_verbosity = RTI_PERSISTENCE_SERVICE_LOG_VERBOSITY_INFO;
    property.dds_verbosity = RTI_PERSISTENCE_SERVICE_LOG_VERBOSITY_WARNINGS;

    /* Initialize the globals */
    RTI_PersistenceService_initialize_globals();

    service = RTI_PersistenceService_new(&property);
    if (service == NULL) {
        printf("Error creating the Persistence Service instance\n");
        return -1;
    }

    ok = RTI_PersistenceService_start(service);
    if (!ok) {
        printf("Error starting the Persistence Service instance\n");
        return -1;
    }

    printf("DomainID=%d, ConfigurationName=%s\n", domainId, property.cfg_name);
    /* Infinite loop until the user terminates the process */
    while (1) {
        NDDS_Utility_sleep(&sleepPeriod);
    }
}

int main(int argc, char *argv[])
{
    int domainId = 0;
    int isPersistent = 0;
    int i = 0;

    for (i = 1; i < argc;) {
        char *param = argv[i++];

        if (strcmp(param, "-domain_id") == 0 && i < argc) {
            domainId = atoi(argv[i++]);
        } else if (strcmp(param, "-persistent") == 0 && i < argc) {
            isPersistent = atoi(argv[i++]);
        } else {
            printf("%s [options]\n"
                    "\t-domain_id <domain ID> (default: 0)\n"
                    "\t-persistent <1 if persistent durability should be used> "
                    "(default: 0 (transient))\n",
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

    return service_main(domainId, isPersistent);
}