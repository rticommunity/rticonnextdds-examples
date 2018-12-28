# Example Code: Time Based Filters

## Concept
The `TIME_BASED_FILTER` QoS Policy allows you to specify that data should not be
delivered more than once per specified period for data-instances of a
*DataReader*, regardless of how fast *DataWriters* are publishing new samples of
the data-instance.

This QoS policy allows you to optimize resource usage (CPU and possibly network
bandwidth) by only delivering the required amount of data to different
*DataReaders*.

*DataWriters* may send data faster than needed by a *DataReader*. For example, a
*DataReader* of sensor data that is displayed to a human operator in a GUI
application does not need to receive data updates faster than a user can
reasonably perceive changes in data values. This is often measure in tenths
(0.1) of a second up to several seconds. However, a *DataWriter* of sensor
information may have *DataReaders* that are processing the sensor information to
control parts of the system and thus need new data updates in measures of
hundredths (0.01) or thousandths (0.001) of a second.

With this QoS policy, different *DataReaders* can set their own time-based
filters, so that data published faster than the period set by a *DataReader*
will be dropped by the middleware and not delivered to the *DataReader*.
Note that all filtering takes place on the reader side.

## Example Description
In this example, the Publisher application publishes samples of two different
instances: `instance0` and `instance1`. Samples of `instance0` are published
every 0.25 seconds, whereas samples of `instance1` are published every
0.5 seconds.

The Subscriber sets a time-based filter with a minimum separation of 2 seconds.
Samples arriving earlier than this are silently dropped. The minimum separation
is determined per-instance, so samples from different instances may arrive
faster than the minimum separation time.

Although filtering may be done by the writer, this is not guaranteed, so
implementing a time-based filter will not necessarily reduce network traffic.
Asynchronous publishing and flow controllers, however, will directly limit the
bandwidth a writer can use.

This QoS is not request-offered, as there is no corresponding QoS on the
*DataWriter* side. However, to be consistent within a *DataReader* QoS, the
`minimum_separation` must be less than any requested deadline. Thus, there is an
indirect dependence on the *DataWriter* settings.

In the example we have set the time-based filter QoS via XML. That is, we have
set the following default QoS in the *USER_QOS_PROFILES.xml* file:
```xml
<datareader_qos>
    <time_based_filter>
        <minimum_separation>
            <sec>2</sec>
            <nanosec>0</nanosec>
        </minimum_separation>
    </time_based_filter>
</datareader_qos>
```

If you want to set the time-based filter QoS programmatically, you will find
that code commented out in the subscriber application.
