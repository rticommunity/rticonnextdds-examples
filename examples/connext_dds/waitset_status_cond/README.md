# Example Code: WaitSet with Status Condition

## Concept
*Conditions* and *WaitSets* provide another way for *Connext* to communicate
status changes (including the arrival of data) to your application. While a
*Listener* is used to provide a callback for asynchronous access, *Conditions*
and *WaitSets* provide synchronous data access. In other words, *Listeners* are
*notification-based* and *Conditions* are *wait-based*.

A *WaitSet* allows an application to wait until one or more attached Conditions
becomes true (or until a timeout expires).

Briefly, your application can create a WaitSet, attach one or more Conditions to
it, then call the WaitSet's `wait()` operation. The `wait()` blocks until one or
more of the WaitSet's attached Conditions becomes TRUE.

A Condition has a trigger_value that can be TRUE or FALSE. You can retrieve the
current value by calling the Condition's only operation, `get_trigger_value()`.

There are three kinds of Conditions. This is a simple example that shows how
to receive notifications about data becoming available using a
*StatusCondition*. This is the most efficient way to use WaitSets to be notified
that you have new data.

StatusConditions are created automatically by *Connext*, one for each Entity. A
StatusCondition is triggered by *Connext* when there is a change to any of that
Entity's enabled statuses.

If you would like more information about the different types of conditions
you can use, look at the "waitsets" example.

A WaitSet can be associated with more than one Entity (including multiple
DomainParticipants). It can be used to wait on Conditions associated with
different DomainParticipants. A WaitSet can only be in use by one application
thread at a time.

## Example Description
This example shows how to use WaitSets to efficiently take data. To initialize
the WaitSet, we define a set of status conditions on which to wait, and attach
them to the WaitSet.

Note that a WaitSet is a "top-level" entity, not one created by a DDS entity.
It is only related to a reader by the contained conditions, which are created
via DataReaders.

Also note that WaitSets block in the context of the application rather than
DDS threads. Thus, there are fewer constraints on processing data than if
we're working in an on_data_available callback, since DDS housekeeping threads
continue to run.
