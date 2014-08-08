=================================================
 Example Code -- WaitSets using query conditions
=================================================

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
view-states, and instance-states. A QueryCondition is a special ReadCondition 
that allows you to specify a query expression and parameters, so you can filter 
on the locally available (already received) data. QueryConditions use the same 
SQL-based filtering syntax as ContentFilteredTopics for query expressions, 
parameters, etc. Unlike ContentFilteredTopics, QueryConditions are applied to 
data already received, so they do not affect the reception of data.
    - StatusConditions are created automatically by Connext, one for each 
Entity. A StatusCondition is triggered by Connext when there is a change to any 
of that Entity's enabled statuses.

A WaitSet can be associated with more than one Entity (including multiple 
DomainParticipants). It can be used to wait on Conditions associated with 
different DomainParticipants. A WaitSet can only be in use by one application 
thread at a time.

Example Description
-------------------
In this example we send a number called 'x' and a string called 'name'. The 
publisher will send the number and the name which is:
    - "EVEN" if the number is even.
    - "ODD" if the number is odd.

In the subscriber, like we are using WaitSet, we do not need to use the 
listeners, but we have to follow some steps to create the WaitSet.
    - Creating the Conditions we are going to attach to the WaitSet.
    - Creating the Waitset.
    - Attaching the conditions to the WaitSet.
    - Waiting for data. 
    
When we are waiting for data, the WaitSet blocks the execution of the thread 
when we call its wait() function until one or more attached Conditions become 
true, or until a user-specified timeout expires.
    
This WaitSet uses a QueryCondition. This QueryCondition is:
    "name MATCH %0"
where %0 is a parameter. 

Firstly, when we create the QueryCondition we set the parameter to the "EVEN" 
value. After 7 sec, we modify the QueryCondition and the parameter has the "ODD"
value.  
      
So, the subscriber receives the EVEN numbers in the first 7 sec of the
execution, and then it receives the ODD numbers. Maybe it can receive a previous
ODD number because we are using KEEP_LAST_HISTORY_QOS with depth=1 in the 
datareader_qos. That is why a DataReader with WaitSet receives all the samples
and it is woken up when a sample satisfies the conditions. However, if a 
DataReader has stored some samples which satisfies the WaitSet's conditions, 
they will be processed too.
