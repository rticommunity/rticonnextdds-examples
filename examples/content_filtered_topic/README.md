# Example Code: Content Filtered Topic

## Concept
A *Content Filtered Topic* is a *Topic* with filtering properties. It makes it
possible to subscribe to topics and at the same time specify that you are only
interested in a subset of the *Topic's* data. For example, suppose you have a
*Topic* that contains a temperature reading for a boiler, but you are only
interested in temperatures outside the normal operating range. A *Content
Filtered Topic* can be used to limit the number of data samples a *DataReader*
has to process and may also reduce the amount of data sent over the network.

A *Content Filtered Topic* creates a relationship between a Topic, also called
the related topic, and user-specified filtering properties. The filtering
properties consist of an expression and a set of parameters.

- The filter expression evaluates a logical expression on the Topic content. The
filter expression is similar to the `WHERE` clause in a *SQL* expression.

- The parameters are strings that give values to the *parameters* in the filter
expression. There must be one parameter string for each parameter in the filter
expression.

## Example Description
In this example, the publisher application reliably transmits 0, 1, 2, .. 9 and
repeats. The last twenty samples are saved for late joiners.

After this topic is created, a *Content Filtered Topic* is created based on the
normal topic. In this case, the filter expression is parameterized:
`(x >= %0 and x <= %1)`, but it could be a static string as well, such as
`x = 3`. The filter parameters, represented by `%k` (where `k = 0 to 99`), are
passed in as a *DDS_StringSeq* when the *Content Filtered Topic* is created.

These parameters can be changed at runtime via `set_expression_parameters()`. In
the example, we modify the expression parameters to three different values.

- When the application starts, the expression parameters are set to
`%0=1 and %1=4` (i.e., we will only receive samples where `1 <= x <=4`).

- After 10 seconds, we change the parameters to `%0=5 and %1=9` (i.e., we will
only receive samples where `5 <= x <= 9`).

- Finally, after 30 seconds, we change set `%0=3 and %1=9` (i.e., we will only
receive samples where `3 <= x <= 9`). 

The first time, the sequence is modified directly. The second change shows how
to get the current parameters, and creates a new sequence.
