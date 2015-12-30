# Example Code: C++11 lambda content filter

With this example you’ll learn how to implement a [`ContentFilter`](https://community.rti.com/static/documentation/connext-dds/current/doc/api/connext_dds/api_cpp2/classrti_1_1topic_1_1ContentFilter.html),
that is, a user-defined filter that can be used to create a [`ContentFilteredTopic`](https://community.rti.com/static/documentation/connext-dds/current/doc/api/connext_dds/api_cpp2/classdds_1_1topic_1_1ContentFilteredTopic.html) or a
[`QueryCondition`](https://community.rti.com/static/documentation/connext-dds/current/doc/api/connext_dds/api_cpp2/classdds_1_1sub_1_1cond_1_1QueryCondition.html). The code is completely generic and you can use it to easily
create efficient lambda content filters.

## Concept
*Connext DDS* provides built-in content filters, most notably the *SQL filter*.
This filter parses ("compiles") a SQL string expression to create a set of steps
that, when executed on each data sample, determine if it passes the filter or
not. The *SQL filter* main advantage is its ease of use and flexibility: you
just need to type a SQL expression. However it is prone to runtime
errors—*Connext DDS* detects syntactic or semantic errors in the expression
when it parses it—and relatively inefficient.

The lambda filter in this example provides similar ease of use and even greater
flexibility—the lambda function can run arbitrary code. Even better, the lambda
filter is defined by actual C++ code, so the compiler will detect errors and it
can be more efficient. It has one disadvantage, though. `DataWriters` can discover
the SQL filter expression used by a `DataReader` and automatically filter
samples before delivering them. With the lambda filter we need to explicitly
register the same filter on the publication side if we want writer-side
filtering.

## Example description
- `LambdaFilter.hpp` implements the generic `LambdaFilter` class
(an implementation of `ContentFilter`) and two helper functions:
`create_lambda_filter` and `create_lambda_cft`.

- `LambdaFilterExample_subscriber.cxx` implements a subscribing application
that creates a `DataReader` with a `ContentFilteredTopic` for the `Stock` type.
This is how you create a `DataReader` with a lambda-based `ContentFilteredTopic`:
    ```
    dds::domain::DomainParticipant participant(domain_id);

    dds::topic::Topic<Stock> topic(participant, "Example Stock");

    auto lambda_cft = create_lambda_cft<Stock>(
        "stock_cft",
        topic,
        [](const Stock& stock)
        {
            return stock.symbol() == "GOOG" || stock.symbol() == "IBM";
        }
    );

    dds::sub::DataReader<Stock> reader(
        dds::sub::Subscriber(participant), lambda_cft);
    ```

- `LambdaFilterExample_publisher.cxx` implements an application that publishes
`Stock` samples each with one of four random symbols (two of which pass the
filter). It also registers the lambda filter to perform writer-side filtering.

You can simply include `LambdaFilter.hpp` in your own *Connext DDS* application
to start using your own lambda filters.
