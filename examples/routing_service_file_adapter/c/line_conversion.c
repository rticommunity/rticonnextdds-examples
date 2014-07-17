/*******************************************************************************
 (c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "line_conversion.h"
#include "data_structures.h"

/* ========================================================================= */
/*                                                                           */
/* Read line                                                                 */
/*                                                                           */
/* ========================================================================= */

int RTI_RoutingServiceFileAdapter_read_sample(
        struct DDS_DynamicData * sampleOut,
        FILE * file,
        RTI_RoutingServiceEnvironment * env)
{
    DDS_Octet * ptr_payload = NULL;
    struct DDS_OctetSeq payload_sequence;
    long data_read = 0;
    DDS_ReturnCode_t retCode = DDS_RETCODE_OK;

    DDS_OctetSeq_initialize(&payload_sequence);
    DDS_OctetSeq_ensure_length(&payload_sequence,
            MAX_PAYLOAD_SIZE, MAX_PAYLOAD_SIZE);
    ptr_payload = DDS_OctetSeq_get_contiguous_buffer(&payload_sequence);
    data_read=fread(ptr_payload,1,MAX_PAYLOAD_SIZE,file);
    if (data_read == 0) {
        if (ferror(file)) {
            fprintf(stderr,"ERROR: reading file");
            return 0;
        }
    }
    DDS_OctetSeq_ensure_length(&payload_sequence, data_read, MAX_PAYLOAD_SIZE);
    DDS_DynamicData_set_octet_seq(sampleOut,
            "value",
            DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,
            &payload_sequence);

    if (retCode != DDS_RETCODE_OK) {
        DDS_OctetSeq_finalize(&payload_sequence);
        return 0;
    }
    DDS_OctetSeq_finalize(&payload_sequence);
    return 1;
}

/* ========================================================================= */
/*                                                                           */
/* Write line                                                                */
/*                                                                           */
/* ========================================================================= */

int RTI_RoutingServiceFileAdapter_write_sample(
        struct DDS_DynamicData * sample,
        FILE * file,
        RTI_RoutingServiceEnvironment * env)
{
    DDS_ReturnCode_t retCode = DDS_RETCODE_OK;
    struct DDS_DynamicDataMemberInfo info;
    DDS_UnsignedLong ulongValue = 0;
    DDS_Octet * ptr_payload = NULL;
    struct DDS_OctetSeq payload;
    DDS_Long sample_length=0;

    DDS_OctetSeq_initialize(&payload);

    DDS_DynamicData_get_octet_seq(sample,
            &payload,
            "value",
            DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);

    sample_length=DDS_OctetSeq_get_length(&payload);
    ptr_payload=DDS_OctetSeq_get_contiguous_buffer(&payload);
    /*it  writes the retrieved string to the file*/
    if (retCode == DDS_RETCODE_OK) {
        if (fwrite(ptr_payload,1,sample_length,file) < sample_length) {
            return 0;
        }

    }
    DDS_OctetSeq_finalize(&payload);

    return 1;
}
