# Example Code: Partitions

If you haven't used the RTI Connext Python API before, first check the
[Getting Started Guide](https://community.rti.com/static/documentation/connext-dds/7.0.0/doc/manuals/connext_dds_professional/getting_started_guide/index.html).

## Running the Example

In two separate command prompt windows for the publisher and subscriber run the
following commands from the example directory (this is necessary to ensure the
application loads the QoS defined in *USER_QOS_PROFILES.xml*):

```sh
python partitions_program.py -p
python partitions_program.py -s
```

Note: you can run multiple publishers and subscribers at the same time.

For the full list of arguments:

```sh
python partitions_program.py -h
```
