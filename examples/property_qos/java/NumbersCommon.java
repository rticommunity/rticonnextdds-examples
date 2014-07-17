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
/* numbers_common.java
 *
 * This file contains the body of the numbers_common_create_participant 
 * function.
 */
 
import com.rti.dds.domain.*;
import com.rti.dds.infrastructure.*;
import com.rti.dds.publication.*;
import com.rti.dds.topic.*;
import com.rti.ndds.config.*;

public class NumbersCommon {
	
/************************************************************************
 * The following code modifies the transport QoS using the 
 * PropertyQoS, introduced in RTI DDS 4.2.
 ************************************************************************/
	
	public static DomainParticipant numbers_common_create_participant(int domainId){
		
		DomainParticipantQos participantQos = new DomainParticipantQos();		
		DomainParticipant part = null;
		
		/* To customize participant QoS, use 
    	DomainParticipantFactory.TheParticipantFactory.get_default_participant_qos() */		
		
		DomainParticipantFactory.TheParticipantFactory.
			get_default_participant_qos(participantQos);
		if (participantQos == null) {
			System.err.println("get default participant qos error\n");
			return null;
		}
				
		/* Set the send socket buffer size */		
		try{
			com.rti.dds.infrastructure.PropertyQosPolicyHelper.add_property(participantQos.property,
					"dds.transport.UDPv4.builtin.send_socket_buffer_size",
					"65507",
					false);
		} catch (RETCODE_ERROR err)
		{
			System.err.println("set send socket buffer size transport property error\n");
 			return null;	
		}
		
		/* Set the receive socket buffer size */
		try{
			
			com.rti.dds.infrastructure.PropertyQosPolicyHelper.add_property(participantQos.property,
					"dds.transport.UDPv4.builtin.recv_socket_buffer_size",
					"65507",
					false);
		} catch (RETCODE_ERROR err)
		{
			System.err.println("set receive socket buffer size transport property error\n");
 			return null;	
		}		 
		
		/* Create the DomainParticipant */
		part = DomainParticipantFactory.TheParticipantFactory.create_participant(domainId, participantQos,
			null /* listener */, StatusKind.STATUS_MASK_NONE);
		if (part == null){
			System.err.println("create_participant error\n");
			return null;
		}
		
		return part;
	}
	
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
 
 	public static DomainParticipant classic_numbers_common_create_participant(int domainId){
 		
 		DomainParticipantFactoryQos factoryQos = new DomainParticipantFactoryQos();
 		DomainParticipant part = null;
 		com.rti.ndds.transport.UDPv4Transport.Property_t transportProperty = new com.rti.ndds.transport.UDPv4Transport.Property_t();
 		 		
 		/* Get the DomainParticipantFactory QoS */
 		DomainParticipantFactory.TheParticipantFactory.get_qos(factoryQos);
 		if (factoryQos == null){
 			System.err.println("get participant factory qos error\n");
 			return null;
 		}
 		
 		/* Turn off auto-enabling of entities */
 		factoryQos.entity_factory.autoenable_created_entities = false;
 		
 		/* Set the DomainParticipantFactory QoS */
 		try { 		
 			DomainParticipantFactory.TheParticipantFactory.set_qos(factoryQos);
 		} catch (RETCODE_ERROR err)
 		{
 			System.err.println("set participant factory qos error\n");
 			return null;
 		}
 		
 		/* Create the DomainParticipant */
 		part = DomainParticipantFactory.TheParticipantFactory.create_participant(domainId,
 			DomainParticipantFactory.PARTICIPANT_QOS_DEFAULT,
			null /* listener */, StatusKind.STATUS_MASK_NONE);
		if (part == null) {		
			System.err.println("create_participant error\n");
			return null;
		}
		
		/* Get current transport QoS */
		try{
			com.rti.ndds.transport.TransportSupport.get_builtin_transport_property(part,transportProperty);
 		} catch (RETCODE_ERROR err)
 		{
 			System.err.println("get builtin transport property error");
 			return null;	
 		}
 		
 		/* Modify the transport QoS */
 		transportProperty.send_socket_buffer_size = 65507;
 		transportProperty.recv_socket_buffer_size = 65507;
 		
 		
		/* Set the transport QoS */
 		try{
			com.rti.ndds.transport.TransportSupport.set_builtin_transport_property(part,transportProperty);
 		} catch (RETCODE_ERROR err)
 		{
 			System.err.println("set builtin transport property error\n");
 			return null;	
 		}
 		
 		/* Enable the DomainParticipant */
 		try{
 			part.enable();
 		} catch (RETCODE_ERROR err)
 		{
 			System.err.println("enable participant error\n");
 			return null;	
 		}
 		
 		return part;
 	}
 	
 /************************************************************************
 * The following code read back the transport QoS (accessing it through
 * the DDS transport QoS) and verify that are the same as the one set 
 * before.
 ************************************************************************/		

	public static int numbers_common_verify_qos(DomainParticipant part) {
		
		com.rti.ndds.transport.UDPv4Transport.Property_t transportProperty = new com.rti.ndds.transport.UDPv4Transport.Property_t();
		
		try{
			com.rti.ndds.transport.TransportSupport.get_builtin_transport_property(part,transportProperty);
 		} catch (RETCODE_ERROR err)
 		{
 			System.err.println("get builtin transport property error\n");
 			return -1;	
 		}
 		
 		if (transportProperty.send_socket_buffer_size != 65507) {
 			System.err.println("Error, send_socket_buffer_size...not modified "+transportProperty.send_socket_buffer_size);
 			return -1;		
 		} else {
 			System.err.println("Ok, send_socket_buffer_size....modified");
 		}
 		
 		if (transportProperty.recv_socket_buffer_size != 65507) {
 			System.err.println("Error, recv_socket_buffer_size...not modified");
 			return -1;		
 		} else {
 			System.err.println("Ok, recv_socket_buffer_size....modified");
 		}
 		
 		System.err.println("New UDPv4 send socket buffer size is:" + transportProperty.send_socket_buffer_size);
 		System.err.println("New UDPv4 receive socket buffer size is:" + transportProperty.recv_socket_buffer_size); 		
 		return 0;
	}
	
}