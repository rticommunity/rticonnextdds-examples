# Tutorial: Data Persistence

This code is part of the Connext Application Design tutorial and is included
here in full for convenience.
Please see the tutorial for instructions.

This code targets Python 3.8 and uses the Connext Python API.

## How to build

The example already provides generated types (`VehicleModeling.py`), so nothing
needs to be done for Python applications.

## How to run

The publisher can be run as a file or module. The expected output contains a
description of the application. It will end once the vehicle has run out of fuel.

```console
$ python publisher.py
Running simulation: simulation=Simulation(self._metrics_writer=<rti.connextdds.DataWriter object at 0x7f7e693cdb30>, self._transit_writer=<rti.connextdds.DataWriter object at 0x7f7e697d83f0>, self._vehicle_vin='3P524A8JB256YZOQY', self._vehicle_fuel=100.0, self._vehicle_route=[Coord(lat=10.851231171123665, lon=4.549741897319626), Coord(lat=-19.336436875210726, lon=45.5761709545348), Coord(lat=-28.473080284108043, lon=-3.3714508132107524), Coord(lat=-44.45875114267031, lon=11.536506807784141), Coord(lat=-30.68510788813299, lon=3.7515235253391954), Coord(lat=-45.485688894791096, lon=-19.45044753805537)], self._vehicle_position=Coord(lat=-9.361148743978752, lon=10.532865637410172))
Vehicle '3P524A8JB256YZOQY' has reached its destination, now moving to a new location...
Vehicle '3P524A8JB256YZOQY' has reached its destination, now moving to a new location...
Vehicle '3P524A8JB256YZOQY' ran out of fuel!

```

The subscriber can be run as a file or a module. The expected output contains a
description of the application, followed by periodic updates on the understanding
of the system. It will run until interrupted.

```console
$ python subscriber.py
[[ DASHBOARD: 2024-07-11 13:28:39.503181 ]]
Online vehicles: 1
- Vehicle F01UV8KCSAY5EVBL3:
  Fuel updates: 4
  Last known destination: Coord(lat=-29.739379575147705, lon=8.811412382546946)
  Last known fuel level: 90.24212596289948
Offline vehicles: 1
- Vehicle V93YZUZIYQB71P77Z:
  Mean fuel consumption: 2.5009597229252654
  Known reached destinations: 3
    - Coord(lat=-19.42303588068077, lon=4.543146614919191)
    - Coord(lat=-23.903966240016082, lon=-1.5759817176247415)
    - Coord(lat=-27.18890271220428, lon=-23.879609290719095)
```
