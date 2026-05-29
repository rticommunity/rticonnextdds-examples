
# (c) Copyright, Real-Time Innovations, 2022.  All rights reserved.
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the software solely for use with RTI Connext DDS. Licensee may
# redistribute copies of the software provided that all such copies are subject
# to this license. The software is provided "as is", with no warranty of any
# type, including any warranty for fitness for any purpose. RTI is under no
# obligation to maintain or support the software. RTI shall not be liable for
# any incidental or consequential damages arising out of the use or inability
# to use the software.

import time
import sys
import rti.connextdds as dds

from numbers import numbers

class numbersPublisher:

    NEW_SOCKET_BUFFER_SIZE_STRING = "65507"

    @staticmethod
    def create_participant(domain_id: int):
        
        participant_qos = dds.DomainParticipantQos()
        
        # Set the send socket buffer size
        participant_qos.property["dds.transport.UDPv4.builtin.send_socket_buffer_size"] = numbersPublisher.NEW_SOCKET_BUFFER_SIZE_STRING

        # Set the recv socket buffer size
        participant_qos.property["dds.transport.UDPv4.builtin.recv_socket_buffer_size"] = numbersPublisher.NEW_SOCKET_BUFFER_SIZE_STRING

        # Create the participant
        participant = dds.DomainParticipant(domain_id, participant_qos)

        return participant


    @staticmethod
    def check_participant(participant: dds.DomainParticipant):
        participant_qos = participant.qos
        send_socket = participant_qos.property.get("dds.transport.UDPv4.builtin.send_socket_buffer_size")
        recv_socket = participant_qos.property.try_get("dds.transport.UDPv4.builtin.recv_socket_buffer_size")

        if send_socket is None:
            print("Error impossible to find send_socket_buffer_size")
            return False
        elif send_socket != numbersPublisher.NEW_SOCKET_BUFFER_SIZE_STRING:
            print("Error, send_socket_buffer_size...not modified")
            return False
        else:
            print("Ok, send_socket_buffer_size....modified")

        if recv_socket is None:
            print("Error impossible to find recv_socket_buffer_size")
            return False
        elif recv_socket != numbersPublisher.NEW_SOCKET_BUFFER_SIZE_STRING:
            print("Error, recv_socket_buffer_size...not modified")
            return False
        else:
            print("Ok, recv_socket_buffer_size....modified")
        
        print(f"New UDPv4 send socket buffer size is: {send_socket}")
        print(f"New UDPv4 receive socket buffer size is: {recv_socket}")
        
        return True

    @staticmethod
    def run_publisher(domain_id: int, sample_count: int):

        # A DomainParticipant allows an application to begin communicating in
        # a DDS domain. Typically there is one DomainParticipant per application.
        # DomainParticipant QoS is configured in USER_QOS_PROFILES.xml
        participant = dds.DomainParticipant(domain_id)

        # If you want to change the DomainParticipant's QoS programmatically
        # rather than using the XML file, you will need to add the following lines
        # to your code and comment out the create_participant call above.
        
        # In this case, we set the transport settings in the XML by default, but
        # in the create_participant call, we set up the transport
        # properties using the Properties QoS in code.

        # participant = numbersPublisher.create_participant(domain_id)

        if(numbersPublisher.check_participant(participant) != True):
            print("check_participant error")

        # A Topic has a name and a datatype.
        topic = dds.Topic(participant, "Example numbers", numbers)

        # This DataWriter will write data on Topic "Example numbers"
        # DataWriter QoS is configured in USER_QOS_PROFILES.xml
        writer = dds.DataWriter(participant.implicit_publisher, topic)
        sample = numbers()    

        sample.number = 1000
        sample.halfNumber = sample.number / 2    

        for count in range(sample_count):
            # Catch control-C interrupt
            try:
                print(f"Writing numbers, count {count}")
                writer.write(sample)

                sample.number = int(sample.halfNumber)
                sample.halfNumber = sample.number / 2

                time.sleep(1)
            except KeyboardInterrupt:
                break

        print("preparing to shut down...")


if __name__ == "__main__":
    numbersPublisher.run_publisher(
            domain_id=0,
            sample_count=sys.maxsize)
