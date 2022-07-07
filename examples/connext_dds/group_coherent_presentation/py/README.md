# Example Code: Group Coherent Presentation

If you haven't used the RTI Connext Python API before, first check the
[Getting Started Guide](https://community.rti.com/static/documentation/connext-dds/6.0.1/api/connext_dds/api_python/index.html).

## Running the Example

In two separate command prompt windows for the publisher and subscriber run the
following commands from the example directory (this is necessary to ensure the
application loads the QoS defined in *USER_QOS_PROFILES.xml*):

```sh
python GroupCoherentExample_publisher.py
python GroupCoherentExample_subscriber.py
```

For the full list of arguments:

```sh
python GroupCoherentExample_publisher.py -h
or
python GroupCoherentExample_subscriber.py -h
```
