===========================================
 Example Code -- WaitSets
===========================================

Concept
-------
Conditions and WaitSets provide another way for Connext to communicate status 
changes (including the arrival of data) to your application. While a Listener 
is used to provide a callback for asynchronous access, Conditions and WaitSets 
provide synchronous data access. In other words, Listeners are 
notification-based and Conditions are wait-based.

A WaitSet allows an application to wait until one or more attached Conditions 
becomes true (or until a timeout expires).

Briefly, your application can create a WaitSet, attach one or more Conditions to
it, then call the WaitSet's wait() operation. The wait() blocks until one or 
more of the WaitSet's attached Conditions becomes TRUE.

A Condition has a trigger_value that can be TRUE or FALSE. You can retrieve the
current value by calling the Condition's only operation, get_trigger_value().

There are three kinds of Conditions. A Condition is a root class for all the 
conditions that may be attached to a WaitSet. This basic class is specialized 
in three classes:

- GuardConditions are created by your application. Each GuardCondition has a 
single, user-settable, boolean trigger_value. Your application can manually 
trigger the GuardCondition by calling set_trigger_value(). Connext does not 
trigger or clear this type of condition -- it is completely controlled by your 
application.
- ReadConditions and QueryConditions are created by your application, but 
triggered by Connext. ReadConditions provide a way for you to specify the 
data samples that you want to wait for, by indicating the desired sample-states,
view-states, and instance-states1.
- StatusConditions are created automatically by Connext, one for each Entity. A 
StatusCondition is triggered by Connext when there is a change to any of that 
Entity's enabled statuses.

A WaitSet can be associated with more than one Entity (including multiple 
DomainParticipants). It can be used to wait on Conditions associated with 
different DomainParticipants. A WaitSet can only be in use by one application 
thread at a time.

Example Description
-------------------
This example shows how to use WaitSets to read data. To initialize the WaitSet, 
we define a set of read and status conditions on which to wait, and attach them 
to the WaitSet.

WaitSets are completely independent of publishers. For this example, we decrease
the liveliness lease duration to trigger the subscriber's StatusCondition.

Note that a WaitSet is a "top-level" entity, not one created by a DDS entity. 
It is only related to a reader by the contained conditions, which are created 
via DataReaders. 

Also note that WaitSets block in the context of the application rather than 
DDS threads. Thus, there are fewer constraints on processing data than if 
we're working in an on_data_available callback, since DDS housekeeping threads 
continue to run. 

