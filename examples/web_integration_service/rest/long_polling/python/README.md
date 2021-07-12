# Running the Example

To run this example, execute _Web Integration Service_, _RTI Shapes Demo_, and
the Python client as follows.

## Running Web Integration Service

To run _Web Integration Service_, open a terminal and enter the following
command:

```shell
$NDDSHOME/bin/rtiwebintegrationservice \
  -cfgFile /path/to/examples/parent/rest/long_polling/long_polling.xml \
  -cfgName longPolling \
  -enableKeepAlive yes
```

Where:

-   The `-cfgFile` argument loads the appropriate configuration file into _Web
    Integration Service_.
-   The `-cfgName` argument specifies the configuration to be instantiated—in
    this case `longPolling`—which starts the `ShapesDemoApp`. This application
    instantiates a _DomainParticipant_ with _DataReaders_ to read to Square
    topics.
-   The `-enablekeepAlive` argument configures the service to keep open the
    underlying TCP connection between client and server between subsequent
    requests and responses when possible. You can ensure that the connection
    survives the subsequent long-lasting requests by increasing the default
    value of `-keepAliveTimeout` to value greater than the timeout specified in
    `long_polling.py`. Note that the `-keepAliveTimeout` argument expects a
    value in milliseconds.

## Running RTI Shapes Demo

Once you have started _Web Integration Service_, open _RTI Shapes
Demo_ and start publishing Squares of different colors on domain 0.

## Running Python Client Application

To read squares using Long Polling from your Python client application,
open another terminal and enter the following command:

```shell
python /path/to/examples/parent/rest/long_polling/python/main.py
```

## Note

To run this example you will need to install the *requests* package using
`pip`.

```shell
pip install -r requirements.txt
```
