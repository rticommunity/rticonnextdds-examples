# Example Code: Group Coherent Presentation

If you haven't used the RTI Connext Python API before, first check the
[Getting Started Guide](https://community.rti.com/static/documentation/connext-dds/current/doc/manuals/connext_dds_professional/getting_started_guide/index.html).

## Building the Example :wrench:

There is no need to generate code from IDL because this example uses the
`String` builtin type.

## Running the Example

In separate command prompt windows run the following commands from the example
directory:

```sh
python builtin_topics_example_publisher.py --password <required password>
python builtin_topics_example_subscriber.py --password <password>
```

For the full list of arguments pass `-h`.

You can run more than one subscriber with different passwords to see how only
when the subscriber sends the right password the publisher allows it to
receive data.
