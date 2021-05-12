# Running the Example

To run this example, execute _Web Integration Service_, _RTI Shapes Demo_, and
the JavaScript client as follows.

## Running Web Integration Service

To run _Web Integration Service_, open a terminal and enter the following
command:

```shell
$NDDSHOME/bin/rtiwebintegrationservice \
  -cfgFile /path/to/examples/parent/rest/long_polling/long_polling.xml \
  -cfgName longPolling \
  -enableKeepAlive yes \
  -documentRoot /path/to/examples/parent
```

Where:

-   The `-cfgFile` argument loads the appropriate configuration file into _Web
    Integration Service_.
-   The `-cfgName` argument specifies the configuration to be instantiated—in
    this case `longPolling`—which starts the `ShapesDemoApp`. This application
    instantiates a _DomainParticipant_ with _DataReaders_ to read to Square
    topics.
-   The `-documentRoot` argument specifies the folder that _Web Integration
    Service's_ web server will provide when accessing the default URL (i.e.,
    `http://<hostname>:8080`). This specific example can then be found under
    `http://<hostname>:8080/rest/long_polling/js`.
-   The `-enablekeepAlive` argument configures the service to keep open the
    underlying TCP connection between client and server between subsequent
    requests and responses when possible. You can ensure that the connection
    survives the subsequent long-lasting requests by increasing the default
    value of `-keepAliveTimeout` to value greater than the timeout specified in
    `long_polling.js`. Note that the `-keepAliveTimeout` argument expects a
    value in milliseconds.

## Running RTI Shapes Demo

Once you have started _Web Integration Service_, open _RTI Shapes
Demo_ and start publishing Squares of different colors on domain 0.

## Running JavaScript Client Application

To read squares using Long Polling from your JavaScript client application, open
a browser and navigate to `http://<hostname>:8080/rest/long_polling/js`. A
simple table will display the sequence of read using Long Polling on a simple
HTML table.
