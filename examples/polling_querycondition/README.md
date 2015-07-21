# Example Code: Polling With Query Condition## ConceptIn *RTI Connext DDS*, you have the option to query for data that is already inthe *DataReader's* queue using a *QueryCondition* object.  This lets you retrievedata from the queue based on the values of one or more fields inside of thatdata. You do this by:1. Specifying the query that you want to use.

   This either looks like the `WHERE` clause in SQL, such as `x < 34 OR y > 2`
   or it can be a string parameter that can be compared using a `MATCH`
   comparison such as `symbol MATCH 'IBM'`. This query can have parameters to
   it, represented with %0, %1, etc. These parameters can be changed at runtime.

2. Creating the *QueryCondition* object

   You use your *DataReader* to create a *QueryCondition* object, specifying the
   query that you want to run, and parameters to that query.
3. Use the *QueryCondition* to retrieve the data subset

   You can call `read_w_condition()` or `take_w_condition()` to retrieve only
   the data that passes the query from the *DataReader's* cache.
4. Optionally, change the parameters at runtime

   You can update the parameters to your query by calling
   `set_query_parameters`.

Note that a *QueryCondition* requires that the single quotes around a string
are _inside the query condition parameter_. For example:
```c
DDS_StringSeq queryParameters;queryParameters.ensure_length(1,1);

// DON'T FORGET THE SINGLE QUOTES INSIDE THE PARAMETERqueryParameters[0] = DDS_String_dup("'Initial String'");
query_condition = _reader->create_querycondition(DDS_ANY_SAMPLE_STATE,    DDS_ANY_VIEW_STATE, DDS_ALIVE_INSTANCE_STATE,    DDS_String_dup("stringField MATCH %0"), queryParameters);

// ...

// DON'T FORGET THE SINGLE QUOTES INSIDE THE PARAMETERqueryParameters[0] = DDS_String_dup("'Changed String'");_queryForFlights->set_query_parameters(queryParameters);```