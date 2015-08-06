# Example Code: Keyed data

## Concept
Topics come in two flavors: those whose associated data type has specified some
fields as defining the **key**, and those whose associated data type has not.
If the data type has some fields that act as a *key*, the *Topic* essentially
defines a collection of data-instances whose values can be independently
maintained.

Since the key fields are contained within the data structure, *Connext* could
examine the key fields each time it needs to determine which data-instance is
being modified. However, for performance and semantic reasons, it is better
for your application to declare all the data-instances it intends to
modify-prior to actually writing any samples. This is known as registration.


## Example Description
This example illustrates how to use keyed data. In particular, in this example
a single *DataWriter* creates three instances differentiated by the field
`<code>`, which is the field used as key.

These three instances has the following behavior:

* The first instance is continuously written.
* At `t=5` we start writing the second and third instances.
* At `t=10` we unregister the second instance, meaning that this *DataWriter* no
  longer has information to send. By default, unregistering an instance disposes
  it, but we change this in the *DataWriter* QoS.
* At `t=15` we dispose the third instance.

In this example, a single *DataReader* receives the data sent by the
*DataWriter*. Here we show how the reader can access instance state information.
This requires only a few modifications in the `on_data_available` handler.
