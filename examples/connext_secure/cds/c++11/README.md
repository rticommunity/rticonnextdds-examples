# Example Code: Lightweight Security with Cloud Discovery Service

## Building the Example

Remember to set your environment variables with the script in your Connext
installation directory before building.

```sh
cd c++11/
mkdir build
cd build
cmake ..
cmake --build .
```

Note: The build process also copies USER_QOS_PROFILES.xml into the build
directory to ensure that it is loaded when you run the examples within the build
directory.

## Running the example

This example is based on a standard rtiddsgen publisher and subscriber example
code and a basic Cloud Discovery Service configuration. Initial peers are set up
so that communication can only happen in the presence of Cloud Discovery
Service, and in particular the secure CDS configuration profile.
A Wireshark capture is supplied as part of the example.

Run the publisher and subscriber in separate terminal windows.

```sh
./CDS_publisher
```

```sh
./CDS_subscriber
```

Then, start Cloud Discovery Service (in a different terminal) using the
`rticlouddiscoveryservice` script from your installation directory. Configure
CDS by passing the `-cfgFile cds.xml -cfgName secure_cds` options to the
application. You can also use the `insecure_cds` profile to compare the
Wireshark output without lightweight security. Optionally, generate a Wireshark capture.

```sh
rticlouddiscoveryservice -cfgFile cds.xml -cfgName secure_cds
```

To filter traffic going to CDS in Wireshark, use udp.dstport == 9999. To filter
traffic relayed by CDS, use rtps.flag.cloud_discovery_service_announcer == 1.
You will that all traffic is SIGNED when using the secure configuration.
This means that the secure prefix and postfix are present for all Data(p)s.
