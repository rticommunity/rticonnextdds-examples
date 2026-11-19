# Example code: Using Property QoS

If you haven't used the RTI Connext Python API before, first check the
[Getting Started Guide](https://community.rti.com/static/documentation/connext-dds/7.0.0/doc/manuals/connext_dds_professional/getting_started_guide/index.html).

## Building the Example :wrench:

First use **rtiddsgen** to generate the python type file from
`numbers.idl`:

```sh
<install dir>/bin/rtiddsgen -language python -platform universal numbers.idl
```

Where `<install dir>` refers to your RTI Connext installation.

You will see messages that look like:

```plaintext
INFO com.rti.ndds.nddsgen.Main Running rtiddsgen version 4.3.0, please wait ...
WARN com.rti.ndds.nddsgen.emitters.FileEmitter File exists and will not be overwritten : some/path/numbers.py
INFO com.rti.ndds.nddsgen.Main Done
```

This is normal and is only informing you that some of the files that **rtiddsgen**
can generate were already available in the repository.

## Running the Example

In two separate command prompt windows for the publisher and subscriber. Run the
following commands from the example directory (this is necessary to ensure the
application loads the QoS defined in USER_QOS_PROFILES.xml):

```sh
python numbers_publisher.py
python numbers_subscriber.py
```

The applications accept up to two arguments:

1.  The `<domain_id>`. Both applications must use the same domain id in order to
    communicate. The default is 0.

2.  How long the examples should run, measured in samples. The default is
    infinite.

While generating the output below, we used values that would capture the most
interesting behavior.

```plaintext
Publisher Output
================
Ok, send_socket_buffer_size....modified
Ok, recv_socket_buffer_size....modified
New UDPv4 send socket buffer size is: 65507
New UDPv4 receive socket buffer size is: 65507
Writing numbers, count 0
Writing numbers, count 1
Writing numbers, count 2
Writing numbers, count 3

Subscriber Output
=================
Ok, send_socket_buffer_size....modified
Ok, recv_socket_buffer_size....modified
New UDPv4 send socket buffer size is: 65507
New UDPv4 receive socket buffer size is: 65507
numbers subscriber sleeping for 4 sec...
   number: 500
   halfNumber: 250.000000
numbers subscriber sleeping for 4 sec...
   number: 250
   halfNumber: 125.000000
numbers subscriber sleeping for 4 sec...
   number: 125
   halfNumber: 62.500000
numbers subscriber sleeping for 4 sec...
```