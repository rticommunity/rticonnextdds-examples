# Example Code: Listeners

## Concept
The *Listener* class is the abstract base class for all listeners. Each
entity class (*DomainParticipant*, *Topic*, *Publisher*, *DataWriter*,
*Subscriber*, and *DataReader*) has its own derived *Listener* class that add
methods for handling entity-specific statuses. The methods are called by an
internal *RTI Connext DDS* thread when the corresponding status for the *Entity*
changes value.

You can choose which changes in status will trigger a callback by installing a
listener with a bitmask. Bits in the mask correspond to different statuses.
The bits that are true indicate that the listener will be called back when
there are changes in the corresponding status. You can specify a listener
and set its bit-mask before or after you create an *Entity*:

```c
DDS_StatusMask mask = DDS_REQUESTED_DEADLINE_MISSED_STATUS |
                         DDS_DATA_AVAILABLE_STATUS;
datareader =
    subscriber->create_datareader(topic,
                                  DDS_DATAREADER_QOS_DEFAULT,
                                  listener,
                                  mask);

DDS_StatusMask mask = DDS_REQUESTED_DEADLINE_MISSED_STATUS |
                      DDS_DATA_AVAILABLE_STATUS;

datareader->set_listener(listener, mask);
```

## Example Description
This example shows how to define listeners for different entity classes,
adding methods for handling entity-specific statuses.

In the publisher application we create a *DataWriter* listener class that
prints out messages every time an event is triggered. We also create
an inconsistent *Topic* so the reader's handler gets called.

In the subscriber application we implement three listeners for *Participant*,
*Subscriber*, and *DataReader*, respectively.
