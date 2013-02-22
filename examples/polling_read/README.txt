===========================================
 Example Code -- Polling Read
===========================================

Concept
-------
In RTI Connext there are three ways to receive data:

- Your application can explicitly check for new data by calling a DataReader's 
  read() or take() method. This method is also known as polling for data.
- Your application can be notified asynchronously whenever new data samples 
  arrive--this is done with a Listener on either the Subscriber or the 
  DataReader. Connext will invoke the Listener's callback routine when there is
  new data. Within the callback routine, user code can access the data by 
  calling read() or take() on the DataReader. This method is the way for your 
  application to receive data with the least amount of latency.
- Your application can wait for new data by using Conditions and a WaitSet, then
  calling wait(). Connext will block your application's thread until the 
  criteria (such as the arrival of samples, or a specific status) set in the 
  Condition becomes true. Then your application resumes and can access the data 
  with read() or take().
  
In this example we focus on the first of these options -- polling read. 
 
Example Description
-------------------
The example implements a publisher application that sends a random number 
as data every second. A subscriber application polls for data once every five 
seconds and computes the average of all examples received in that period.

Note that using polling read instead of getting data via callbacks, means that 
the processing occurs in the context of the application's main() rather than in
the DDS receive thread.

