/* numbers_common.h
 *
 * This file contains few macros that are used to configure the transport
 * in the following files:
 * - numbers_publisher.c
 * - numbers_subscriber.c
 */

/* Comment the following macro to allow compilation on RTI DDS older than 4.2 */
//#define USE_NDDS42_API

/* The new value of the transport socket size */
#define NEW_SOCKET_BUFFER_SIZE          (65507)
#define NEW_SOCKET_BUFFER_SIZE_STRING   ("65507")





/* Creates the domain participant, modifying the transport qos properties
 * to increase the send and receive socket buffer size to 64k. 
 * Returns zero if success, non-zero in case of error.
 */
extern DDS_DomainParticipant *numbers_common_create_participant(int domainId);


/* Reads the transport QoS and verify that are correctly set
 * Returns zero if success, non-zero in case of error.
 */
extern int numbers_common_verify_qos(DDS_DomainParticipant *part);
