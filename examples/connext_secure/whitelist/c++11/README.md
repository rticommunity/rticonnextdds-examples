# Example Code: Whitelist

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

## Setting up Security artifacts

The build process copies the security folder with .cnf files into the build
directory. Use the provided python script to initialize them. This means that
`build/security` will contain all the security artifacts needed to run this example.

```sh
cd security
python3 setup_security.py
```

## Running the example

This example is based on a standard rtiddsgen publisher and subscriber example
code. This has been altered to perform set_qos operations at runtime in order to
add / remove the subscribers' certificates to / from the publisher's whitelist.
This happens automatically in 30 second increments, so no external user input
is needed to run the example.

Run two instances of the subscriber in separate terminal windows, one with the
CLI parameter --peer3 so it uses a different set of identity certificates.

```sh
./Whitelist_subscriber
```

```sh
./Whitelist_subscriber --peer3
```

The subscriber applications will log an error message when they attempt to
authenticate each other and fail to do so. The reason is that the subscribers
have configured a whitelist with only one entry: the publisher's DomainParticipant.

Then, launch the publisher in a third terminal window.

```sh
./Whitelist_publisher
```

Initially, both subscribers have the publisher in their respective whitelists
and the publisher has both subscribers in its whitelist. The publisher's
whitelist is changed at runtime to cover all possible combinations of two
subscribers, so you can see how the data received changes.

First, both subscribers are in the waitlist, and will both receive all samples:

```sh
Subscription matched: current count: 1
Whitelist subscriber sleeping up to 1 sec...
[value: 2]
Whitelist subscriber sleeping up to 1 sec...
[value: 3]
```

Then, they will be removed from the whitelist and no longer recieve samples.
The publisher will also print a log message:

```sh
Writing Whitelist, count 15
Empty whitelist

ERROR [0x8C8826C0,0x2E4EC87D,0x98017765:0x000001C1|VALIDATE REMOTE PARTICIPANT IDENTITY|CHECK AUTHENTICATION STATUS|LC:SEC]PRESParticipant_onSecurityIdentityEventListener:{"DDS:Security:LogTopicV2":{"f":"10","s":"3","t":{"s":"1699541683","n":"233309000"},"h":"RTISP-10036","i":"0.0.0.0","a":"RTI Secure DDS Application","p":"117701","k":"security","x":[{"DDS":[{"domain_id":"0"},{"guid":"8c8826c0.2e4ec87d.98017765.1c1"},{"plugin_class":"Authentication"},{"plugin_method":"PRESParticipant_onSecurityIdentityEventListener"}]}],"m":"remote participant eeb57cb4.71cef80b.6ff1b8fd identity status revoked by subject name whitelist for local participant 8c8826c0.2e4ec87d.98017765"}}
ERROR [0x8C8826C0,0x2E4EC87D,0x98017765:0x000001C1|VALIDATE REMOTE PARTICIPANT IDENTITY|CHECK AUTHENTICATION STATUS|LC:SEC]PRESParticipant_onSecurityIdentityEventListener:{"DDS:Security:LogTopicV2":{"f":"10","s":"3","t":{"s":"1699541683","n":"234514000"},"h":"RTISP-10036","i":"0.0.0.0","a":"RTI Secure DDS Application","p":"117701","k":"security","x":[{"DDS":[{"domain_id":"0"},{"guid":"8c8826c0.2e4ec87d.98017765.1c1"},{"plugin_class":"Authentication"},{"plugin_method":"PRESParticipant_onSecurityIdentityEventListener"}]}],"m":"remote participant a583f60d.25b380b0.9853f1db identity status revoked by subject name whitelist for local participant 8c8826c0.2e4ec87d.98017765"}}

Writing Whitelist, count 16
Writing Whitelist, count 17
```

The publisher will continue alternating which subscriber is included in the
whitelist. Only the subscriber in the whitelist will receive the samples.
You will see a log message in the publisher application about the authentication
error for the subscriber that is not in the whitelist.

The example finishes by adding both subscribers back to the whitelist.
They should start to receive samples again.


## Troubleshooting

### Compilation fails accessing struct field

If the code compilation fails with errors such as "reference to non-static member
function must be called" for code such as `my_sample.my_field = value` or
`value = my_sample.my_field` this means that the rtiddsgen version you are using
doesn't have the IDL4 C++ mapping enabled by default.

To fix it, upgrade your Connext version to 7.6+ or check out the branch for the
Connext version you're using, e.g.

```sh
git checkout release/7.3.0
```
