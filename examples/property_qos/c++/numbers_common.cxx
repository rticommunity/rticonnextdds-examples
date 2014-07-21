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
/* numbers_common.c
 *
 * This file contains the body of the numbers_common_create_participant 
 * function.
 */


#include <stdio.h>
#include <stdlib.h>
#include "ndds/ndds_cpp.h"
#include "numbers.h"
#include "numbersSupport.h"

#include "numbers_common.h"


#ifdef USE_NDDS42_API
/************************************************************************
 * The following code modifies the transport QoS using the 
 * PropertyQoS, introduced in RTI DDS 4.2.
 ************************************************************************/
DDSDomainParticipant * numbers_common_create_participant(int domainId) {
    struct DDS_DomainParticipantQos domainparticipant_qos = DDS_PARTICIPANT_QOS_DEFAULT;  
    DDS_ReturnCode_t retcode;
    DDSDomainParticipant * part = NULL;

    /* To customize participant QoS, use 
    DDS_DomainParticipantFactory_get_default_participant_qos() */

    retcode = DDSTheParticipantFactory->get_default_participant_qos(domainparticipant_qos);	
    if (retcode != DDS_RETCODE_OK) {
        puts("Error, impossible get default participant qos");
        return NULL;
    }

    /* Set the send socket buffer size */
	retcode = DDSPropertyQosPolicyHelper::add_property(domainparticipant_qos.property,
		"dds.transport.UDPv4.builtin.send_socket_buffer_size" ,
		NEW_SOCKET_BUFFER_SIZE_STRING,
		DDS_BOOLEAN_FALSE);
    if (retcode != DDS_RETCODE_OK) {
        printf("Error, impossible add property");
        return NULL;
    }

    /* Set the receive socket buffer size */
    retcode = DDSPropertyQosPolicyHelper::add_property(domainparticipant_qos.property,
		"dds.transport.UDPv4.builtin.recv_socket_buffer_size" ,
		NEW_SOCKET_BUFFER_SIZE_STRING,
		DDS_BOOLEAN_FALSE);
    if (retcode != DDS_RETCODE_OK) {
        printf("Error, impossible add property");
        return NULL;
    }   

	/* Create the participant */
    part = DDSTheParticipantFactory->create_participant(
	    domainId, domainparticipant_qos,
	    NULL /* listener */, DDS_STATUS_MASK_NONE);
    if (part == NULL) {
        puts("create_participant error");
        return NULL;
    }
    return part;
}


#else

/************************************************************************
 * This is the "classic" method for changing the transport QoS. 
 *
 * It shows how to change QoS without using the 
 * PropertyQoS, which was added in 4.2. 
 * This method is still available in 4.2 and later.
 *
 * To modify the transport QoS using this classic method:
 * - turn off auto-enable using the DomainParticipantFactory QoS
 * - create the DomainParticipant (disabled)
 * - get the transport QoS
 * - modify the transport QoS
 * - set the new transport QoS
 * - enable the DomainParticipant
 * 
 ************************************************************************/
DDSDomainParticipant * numbers_common_create_participant(int domainId) {
	struct NDDS_Transport_UDPv4_Property_t transport_UDPv4_property = NDDS_TRANSPORT_UDPV4_PROPERTY_DEFAULT;
	struct DDS_DomainParticipantFactoryQos factory_qos = DDS_DomainParticipantFactoryQos_INITIALIZER;
	DDS_ReturnCode_t retcode;
	DDSDomainParticipant * part = NULL;

	/* Get the current DomainParticipantFactory QoS */
	retcode = DDSTheParticipantFactory->get_qos(factory_qos);
	if (retcode != DDS_RETCODE_OK) {
		puts("Error, impossible get domain participant factory qos");
		return NULL;
	}

	/* Turn off auto-enabling of entities */
	factory_qos.entity_factory.autoenable_created_entities = DDS_BOOLEAN_FALSE;

	/* Set the DomainParticipantFactory QoS */
	retcode = DDSTheParticipantFactory->set_qos(factory_qos);
	if (retcode != DDS_RETCODE_OK) {
		puts("Error, impossible set domain participant factory qos");
		return NULL;
	}

	/* Create the DomainParticipant */
	part = DDSTheParticipantFactory->create_participant(
		domainId, DDS_PARTICIPANT_QOS_DEFAULT,
	NULL /* listener */, DDS_STATUS_MASK_NONE);
	if (part == NULL) {
		printf("create_participant error\n");
		return NULL;
	}

	/* Get current transport QoS */
	retcode = NDDSTransportSupport::get_builtin_transport_property(part, 
		DDS_TRANSPORTBUILTIN_UDPv4, 
		(struct NDDS_Transport_Property_t &)transport_UDPv4_property);
	if (part == NULL) {
		printf("NDDS_Transport_Support_get_builtin_transport_property error\n");
		return NULL;
	}

	/* Modify the transport QoS */
	transport_UDPv4_property.send_socket_buffer_size = NEW_SOCKET_BUFFER_SIZE;
	transport_UDPv4_property.recv_socket_buffer_size = NEW_SOCKET_BUFFER_SIZE;

	/* Set the transport QoS */
	retcode = NDDSTransportSupport::set_builtin_transport_property(part, 
		DDS_TRANSPORTBUILTIN_UDPv4, 
		(struct NDDS_Transport_Property_t &)transport_UDPv4_property);
	if (retcode != DDS_RETCODE_OK) {
		printf("NDDS_Transport_Support_set_builtin_transport_property error\n");
		return NULL;
	}

	/* Enable the DomainParticipant */
	//retcode = DDS_Entity_enable(DDS_DomainParticipant_as_entity(part));
	retcode = part->enable();
	if (retcode != DDS_RETCODE_OK) {
		printf("DDS_Entity_enable error\n");
		return NULL;
	}
return part;
}
#endif





/************************************************************************
 * The following code read back the transport QoS (accessing it through
 * the DDS transport QoS) and verify that are the same as the one set 
 * before.
 ************************************************************************/
int numbers_common_verify_qos(DDSDomainParticipant * part) {
	struct NDDS_Transport_UDPv4_Property_t transport_UDPv4_property = NDDS_TRANSPORT_UDPV4_PROPERTY_DEFAULT;
	DDS_ReturnCode_t retcode;

        retcode = NDDSTransportSupport::get_builtin_transport_property(part, 
		DDS_TRANSPORTBUILTIN_UDPv4,
                reinterpret_cast<NDDS_Transport_Property_t &>(transport_UDPv4_property));
	if (retcode != DDS_RETCODE_OK) {
		puts("Error, impossible to get builtin transport property");
		return -1;
	} else {
	if (transport_UDPv4_property.send_socket_buffer_size != NEW_SOCKET_BUFFER_SIZE) {
		puts("Error, send_socket_buffer_size...not modified");
		return -1;
	} else {
		puts("Ok, send_socket_buffer_size....modified");
	}
	if (transport_UDPv4_property.recv_socket_buffer_size != NEW_SOCKET_BUFFER_SIZE) {
		puts("Error, recv_socket_buffer_size...not modified");
		return -1;
	} else {
		puts("Ok, recv_socket_buffer_size....modified");
	}
	}
	printf("New UDPv4 send socket buffer size is: %d \n", transport_UDPv4_property.send_socket_buffer_size);
	printf("New UDPv4 receive socket buffer size is: %d \n", transport_UDPv4_property.recv_socket_buffer_size);
	return 0;
}



