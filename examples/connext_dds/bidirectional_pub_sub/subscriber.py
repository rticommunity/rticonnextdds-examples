import time
import rti.connextdds as dds
import rti.types as idl

@idl.struct
class Message:
    text: str = ""

participant = dds.DomainParticipant(0)
publisher_topic = dds.Topic(participant,"topic_a",Message)
subscriber_topic = dds.Topic(participant, "topic_b", Message)

writer = dds.DataWriter(participant.implicit_publisher, publisher_topic)
reader = dds.DataReader(participant.implicit_subscriber, subscriber_topic)

count = 0

print("Process B started")

while True:
    msg = Message(text=f"Hello from B {count}")
    writer.write(msg)
    print("Sent: ",msg.text)

    for data in reader.take_data():
        print("Received: ",data.text)

    count += 1
    time.sleep(1)