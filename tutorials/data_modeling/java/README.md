# Tutorial: Data Modeling

This code is part of the Connext Data Modeling tutorial and is included
here in full for convenience.

This code targets Java 8 or and uses the Connext Java API.

## How to build

The example already provides generated example files
(`VehicleMetricsPublisher.java`, `VehicleMetricsSubscriber.java` and
`Application.java`), but requires generating some additional files.

This functionality is built-in into the Gradle build script, which
itself requires the `NDDSHOME` environment variable to be set:

```console
$ export NDDSHOME=/path/to/connext
```

```console
> set NDDSHOME=\path\to\connext
```


## How to run

The publisher can be run with the `runPublisher` Gradle task. The publisher is a
slighly modified generated example file that publishes "default" samples in a loop.

```console
Buildfile: build.xml

compile:

VehicleMetricsPublisher:
    [java] Writing VehicleMetrics, count 0
    [java] Writing VehicleMetrics, count 1
    [java] Writing VehicleMetrics, count 2
    [java] Writing VehicleMetrics, count 3
```

The subscriber can be run with the `runSubscriber` Gradle task. The subscriber is
a slighly modified generated example file that subscribes to data and displays all its
values as they're received.

```console
$ ant VehicleMetricsSubscriber
Buildfile: build.xml

compile:

VehicleMetricsSubscriber:
    [java] Received:
    [java]     vehicle_vin:
    [java]     fuel_level: 0.0
    [java]
    [java] No data after 1 seconds.
    [java] Received:
    [java]     vehicle_vin:
    [java]     fuel_level: 0.0
    [java]
    [java] Received:
    [java]     vehicle_vin:
    [java]     fuel_level: 0.0
    [java]
    [java] No data after 1 seconds.
    [java] Received:
    [java]     vehicle_vin:
    [java]     fuel_level: 0.0
    [java]
```
