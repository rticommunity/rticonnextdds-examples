# Example Code: Deadlines and Content Filtering

## Concept
In *RTI Connext DDS*, you can use the `DEADLINE` QoS to specify that you expect
updates about a particular instance within a given period of time.

This QoS is request-offered, meaning that the *DataWriter* offers a level of
service, and the *DataReader* requests a level of service. Request-offered QoS
are like a contract between the *DataWriter* and the *DataReader*:
the *DataWriter* is saying that it will provide a level of service, and the
*DataReader* is saying it requires a level of service. The offered level must be
greater than or equal to the requested level, or there will be no communication
between the *DataWriter* and the *DataReader*.

However, the middleware cannot enforce the `DEADLINE` contract, because that is
subject to:

1. how often the application calls `write()`.
2. whether the network loses packets.
3. whether the reader application is filtering some of the data.

So, the middleware is responsible for:

1. Notifying the *DataWriter* application if the *DataWriter* is not writing as
   often as it specified in its `DEADLINE` period.
2. Notifying the *DataReader* application if data did not arrive within its
   `DEADLINE` period.


## Example Description
1. The *DataWriter* writing two instances, both of them fast enough to satisfy
   the `DEADLINE` period.
2. After 10 seconds, the *DataReader* uses a *Content-Filtered Topic* to filter
   out one of those instances. The *DataReader* now gets notifications that it
   is not receiving that instance within the `DEADLINE` period, and prints out
   which instance is not being received on time.
3. After 15 seconds, the *DataWriter* no longer writes one of the instances.
   Now the *DataWriter* is notified that it is failing to meet its `DEADLINE`
   contract for that instance.
