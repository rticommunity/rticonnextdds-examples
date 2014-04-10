/* ========================================================================= */
/* RTI Routing Service File Adapter                                          */
/* ========================================================================= */
/*                                                                           */
/* See LineConversion.c                                                      */
/*                                                                           */
/* ========================================================================= */

#ifndef _lineconversion_h_
#define _lineconversion_h_

#include <stdio.h>
#include <string.h>
#include "ndds/ndds_c.h"
#include "routingservice/routingservice_adapter.h"

/* ========================================================================= */
/*                                                                           */
/* Read line                                                                 */
/*                                                                           */
/* ========================================================================= */

int RTI_RoutingServiceFileAdapter_read_sample(
    struct DDS_DynamicData * sampleOut, 
    FILE * file,
    RTI_RoutingServiceEnvironment * env);


/* ========================================================================= */
/*                                                                           */
/* Write line                                                                */
/*                                                                           */
/* ========================================================================= */

int RTI_RoutingServiceFileAdapter_write_sample(
    struct DDS_DynamicData * sample, 
    FILE * file,
    RTI_RoutingServiceEnvironment * env);



#endif
