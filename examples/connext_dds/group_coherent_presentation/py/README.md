# Example Code: Group Coherent Presentation

If you haven't used the RTI Connext Python API before, first check the
[Getting Started Guide](https://community.rti.com/static/documentation/connext-dds/7.0.0/doc/manuals/connext_dds_professional/getting_started_guide/index.html).

## Building the Example :wrench:

First use **rtiddsgen** to generate the python type file from
`GroupCoherentExample.idl`:

```sh
<install dir>/bin/rtiddsgen -language python -platform universal GroupCoherentExample.idl
```

Where `<install dir>` refers to your RTI Connext installation.

You will see messages that look like:

```plaintext
INFO com.rti.ndds.nddsgen.Main Running rtiddsgen version 3.1.1, please wait ...
WARN com.rti.ndds.nddsgen.emitters.FileEmitter File exists and will not be overwritten : some/path/GroupCoherentExample.py
INFO com.rti.ndds.nddsgen.Main Done
```

This is normal and is only informing you that some of the files that **rtiddsgen**
can generate were already available in the repository.

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
