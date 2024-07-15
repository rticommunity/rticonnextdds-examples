# Tutorial: Data Modeling

This code is part of the Connext Data Modeling tutorial and is included
here in full for convenience.

This code targets Python 3.8 and uses the Connext Python API.

## How to build

The example already provides generated types (`VehicleModeling.py`) and example
files (`VehicleModeling_program.py`, `VehicleModeling_subscriber.py`
and `VehicleModeling_publisher.py`), so nothing needs to be done for
Python applications.

These files were generated using the following command:

```console
$ rtiddsgen \
  -language python \
  -create typefiles \
  -create examplefiles \
  -d . \
  ../VehicleModeling.xml
```

## How to run

The publisher can be run as a file or module. The publisher is a slighly modified
generated example file that publishes "default" samples in a loop.

```console
$ python VehicleModeling_program.py --pub
Running VehicleMetricsPublisher on domain 0
Writing VehicleMetrics, count 0
Writing VehicleMetrics, count 1
Writing VehicleMetrics, count 2

```

The subscriber can be run as a file or a module. The subscriber is a slighly
modified generated example file that subscribes to data and displays all its
values as they're received.

```console
$ python ./VehicleModeling_program.py --sub
Running VehicleMetricsSubscriber on domain 0
Hello World subscriber sleeping for 1 seconds...
Received: VehicleMetrics(vehicle_vin='', fuel_level=0.0)
Hello World subscriber sleeping for 1 seconds...
Hello World subscriber sleeping for 1 seconds...
Received: VehicleMetrics(vehicle_vin='', fuel_level=0.0)
Hello World subscriber sleeping for 1 seconds...
Hello World subscriber sleeping for 1 seconds...
Received: VehicleMetrics(vehicle_vin='', fuel_level=0.0)
```
