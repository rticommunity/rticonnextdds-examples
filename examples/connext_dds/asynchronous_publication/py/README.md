# Example Code: Asynchronous Publishing

If you haven't used the RTI Connext Python API before, first check the
[Getting Started Guide](https://community.rti.com/static/documentation/connext-dds/7.0.0/doc/manuals/connext_dds_professional/getting_started_guide/index.html).

Note that in this example the DDS ``PublishMode`` QoS that allows asynchronous
publication (see the top-level [README](../README.md)) is unrelated to the
DataReader method ``take_data_async``, which is a feature specific to the Python
API that allows the application to receive data in an asynchronous manner.

## Running the Example

In two separate command prompt windows for the publisher and subscriber run the
following commands from the example directory (this is necessary to ensure the
application loads the QoS defined in *USER_QOS_PROFILES.xml*):

```sh
python async_publisher.py
python async_subscriber.py
```

For the full list of arguments:

```sh
python async_publisher.py -h
or
python async_subscriber.py -h
```
