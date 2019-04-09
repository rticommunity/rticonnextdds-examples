# Example Code: Content Filtered Topic - Using String Filter

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
  expression. There must be one parameter string for each parameter in the
  filter expression.

## Example Description
After this *Topic* is created, a *Content Filtered Topic* is created based on
the normal *Topic*. In this case, we will use a parameterized `STRINGMATCH`
filter expression: `name MATCH %0`, but it could be a static string as well,
such as `name MATCH 'RTI'`. The filter parameters, represented by `%k`
(where k = 0 to 99), are passed in as a `DDS_StringSeq` when the *Content
Filtered Topic* is created.

To create this kind of topic, we need to use
`create_contentfilteredtopic_with_filter` as follows:
```c
create_contentfilteredtopic_with_filter(
            "ContentFilteredTopic", topic, "name MATCH %0", parameters,
            DDS_STRINGMATCHFILTER_NAME);
```

Parameters can be changed at runtime via `append_to_expression_parameter()`
and `remove_from_expression_parameter()`. In the example, we modify the
expression parameters to three different values.

- When the application starts, the expression parameters are set to
  `%0="SOME_STRING"`. After that, we will append to expression the parameter
  `EVEN`.

- After 10 seconds, we will append again the parameter `ODD`. So the
  *DataReader* will receive the *EVEN* and *ODD* numbers (all).

- Finally, after 20 seconds, we will remove the parameter `EVEN` and the
  *DataReader* will receive just the *ODD* numbers.
