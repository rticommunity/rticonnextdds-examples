# Example Code: C++11 lambda content filter

With this example you’ll learn how to implement a `CustomFilter` <LINK>, that is,
a user-defined filter that can be used to create a `ContentFilteredTopic` or a
`QueryCondition`. The code is completely generic and you can use it to easily
create efficient lambda content filters.

## Concept
*Connext DDS* provides built-in content filters, most notably the *SQL filter*.
This filter parses a SQL string expression to create a set of steps to execute
for each data sample to determine if it passes the filter or not. The *SQL
filter* main advantage is its ease of use and flexibility. However it is prone
to runtime errors—*Connext DDS* detects syntactic or semantic errors in the
expression when it parses it—and relatively inefficient.

The lambda filter in this example provides the same ease of use and even greater
flexibility—the lambda function can run arbitrary code. Even better, the lambda
filter is defined by actual C++ code, so the compiler will detect errors and it
can be more efficient.

## Example description
- `LambdaFilter.hpp` implements the generic `LambdaFilter` class
(an implementation of `CustomFilter`) and two helper functions:
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
`Stock` samples each with one of four random symbols (two of which pass the filter).

You can simply include `LambdaFilter.hpp` in your own *Connext DDS* application
to start using your own lambda filters.
