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
#ifndef NUMBERS_COMMON_H
#define NUMBERS_COMMON_H

#include "ndds/ndds_cpp.h"
#include "numbers.h"
#include "numbersSupport.h"
#include <stdio.h>
#include <stdlib.h>

/* numbers_common.h
 *
 * This file contains few macros that are used to configure the transport
 * in the following files:
 * - numbers_publisher.c++
 * - numbers_subscriber.c++
 */

/* The new value of the transport socket size */
#define NEW_SOCKET_BUFFER_SIZE (65507)
#define NEW_SOCKET_BUFFER_SIZE_STRING ("65507")


/* Creates the domain participant, modifying the transport qos properties
 * to increase the send and receive socket buffer size to 64k.
 * Returns zero if success, non-zero in case of error.
 */
inline DDSDomainParticipant *numbers_common_create_participant(int domain_id)
{
    struct DDS_DomainParticipantQos domainparticipant_qos =
            DDS_PARTICIPANT_QOS_DEFAULT;
    DDS_ReturnCode_t retcode;
    DDSDomainParticipant *part = NULL;

    /* To customize participant QoS, use
    DDS_DomainParticipantFactory_get_default_participant_qos() */

    retcode = DDSTheParticipantFactory->get_default_participant_qos(
            domainparticipant_qos);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "Error, impossible get default participant qos\n";
        return NULL;
    }

    /* Set the send socket buffer size */
    retcode = DDSPropertyQosPolicyHelper::add_property(
            domainparticipant_qos.property,
            "dds.transport.UDPv4.builtin.send_socket_buffer_size",
            NEW_SOCKET_BUFFER_SIZE_STRING,
            DDS_BOOLEAN_FALSE);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "Error, impossible add property\n";
        return NULL;
    }

    /* Set the receive socket buffer size */
    retcode = DDSPropertyQosPolicyHelper::add_property(
            domainparticipant_qos.property,
            "dds.transport.UDPv4.builtin.recv_socket_buffer_size",
            NEW_SOCKET_BUFFER_SIZE_STRING,
            DDS_BOOLEAN_FALSE);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "Error, impossible add property\n";
        return NULL;
    }

    /* Create the participant */
    part = DDSTheParticipantFactory->create_participant(
            domain_id,
            domainparticipant_qos,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (part == NULL) {
        std::cerr << "create_participant error\n";
        return NULL;
    }
    return part;
}


/* Reads the transport QoS and verify that are correctly set
 * Returns zero if success, non-zero in case of error.
 */
inline int numbers_common_verify_qos(DDSDomainParticipant *part)
{
    struct NDDS_Transport_UDPv4_Property_t transport_UDPv4_property =
            NDDS_TRANSPORT_UDPV4_PROPERTY_DEFAULT;
    DDS_ReturnCode_t retcode;

    retcode = NDDSTransportSupport::get_builtin_transport_property(
            part,
            DDS_TRANSPORTBUILTIN_UDPv4,
            reinterpret_cast<NDDS_Transport_Property_t &>(
                    transport_UDPv4_property));
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "Error, impossible to get builtin transport property\n";
        return -1;
    } else {
        if (transport_UDPv4_property.send_socket_buffer_size
            != NEW_SOCKET_BUFFER_SIZE) {
            std::cerr << "Error, send_socket_buffer_size...not modified\n";
            return -1;
        } else {
            std::cout << "Ok, send_socket_buffer_size....modified\n";
        }
        if (transport_UDPv4_property.recv_socket_buffer_size
            != NEW_SOCKET_BUFFER_SIZE) {
            std::cerr << "Error, recv_socket_buffer_size...not modified\n";
            return -1;
        } else {
            std::cout << "Ok, recv_socket_buffer_size....modified\n";
        }
    }
    std::cout << "New UDPv4 send socket buffer size is: "
              << transport_UDPv4_property.send_socket_buffer_size << std::endl;
    std::cout << "New UDPv4 receive socket buffer size is: "
              << transport_UDPv4_property.recv_socket_buffer_size << std::endl;
    return 0;
}

#endif
