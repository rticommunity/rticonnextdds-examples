# Example Code: Persistence Service

## Persistence Service

In many applications, we need to store data samples so they can be delivered to
subscribing applications that join the system at a later time or to avoid
receiving old data each time we re-join to the same publisher. Or maybe we need
to keep the data available even once our publishing application has terminated.
In *RTI Connext* this behavior can be achieved using *Persistence Service*.

*Persistence Service* is a *Connext* application that saves data samples to
transient or permanent storage, so they can be delivered to subscribing
applications that join the system at a later time - even if the publishing
application has already terminated.

When configured to run in `PERSISTENT` mode, *Persistence Service* uses the
filesystem. For each persistent topic, it collects all the data written by
the corresponding persistent *DataWriters* and stores them into persistent
storage. When configured to run in `TRANSIENT` mode, *Persistence Service*
stores the data in memory.
