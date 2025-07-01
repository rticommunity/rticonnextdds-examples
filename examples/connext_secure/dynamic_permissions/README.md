# Example Code: Dynamic Permissions

## Concept

This example showcases how the Security Plugins enforce Permissions Document
expiration, and how the Permissions Document can be renewed to resume
communication.

## Building the Example

Use the following commands to build the example and get the executables that
you can run:

```sh
cd c++11/
mkdir build && cd build
cmake ..
cmake --build .
```

You can optionally pass the
``-DCONNEXTDDS_DIR=<your_connext_installation_directory>``,
``-DOPENSSL_ROOT_DIR=<your_openssl_installation_directory>``,
``-DCONNEXTDDS_ARCH=<your_architecture>``,
``-DCMAKE_BUILD_TYPE=<Debug/Release>``, and
``-DBUILD_SHARED_LIBS=<ON/OFF>`` variables to the cmake configuration step.

After building the example, you will have a publisher Permissions Document that
expires in 1 minute. If you need to re-create it, please remove this file from
your build directory and re-run the ``createExpiringPermissions`` target.

```sh
rm security/ecdsa01/xml/Permissions2_expiring.xml && \
    cmake --build . --target createExpiringPermissions
```

## Running the example

Demo is based on a standard rtiddsgen publisher and subscriber example code.

Run a publisher and a subscriber in separate terminal windows.

```sh
./dynamic_permissions_publisher
```

```sh
./dynamic_permissions_subscriber
```

Verify that they communicate and that the subscriber is receiving data.

```sh
# Publisher
Writing ::DynamicPermissions, count 0
Writing ::DynamicPermissions, count 1
# [...]

# Subscriber
::DynamicPermissions subscriber sleeping up to 1 sec...
[value: 0]
::DynamicPermissions subscriber sleeping up to 1 sec...
[value: 1]
::DynamicPermissions subscriber sleeping up to 1 sec...
# [...]
```

Once the Permissions Document of the publisher DomainParticipant expires, you
will see the following error messages:

```sh
# Publisher
ERROR [0x831AB06E,0x43876C36,0xFD825600:0x000001C1|ADVANCE NOTIFY INVALID LOCAL PERMISSIONS|CHECK STATUS|LC:Security] RTI_Security_PermissionsGrant_isValidTime:{"DDS:Security:LogTopicV2":{"f":"10","s":"3","t":{"s":"1748517658","n":"108000"},"h":"RTISP-10036","i":"0.0.0.0","a":"RTI Secure DDS Application","p":"85264","k":"50331706","x":[{"DDS":[{"domain_id":"0"},{"guid":"831AB06E.43876C36.FD825600.000001C1"},{"plugin_class":"DDS:Access:Permissions"},{"plugin_method":"RTI_Security_PermissionsGrant_isValidTime"}]}],"m":"now is after not_after of permissions file"}}
ERROR [0x831AB06E,0x43876C36,0xFD825600:0x000001C1|ADVANCE NOTIFY INVALID LOCAL PERMISSIONS|CHECK STATUS|LC:Security] RTI_Security_AccessControl_validate_status:{"DDS:Security:LogTopicV2":{"f":"10","s":"3","t":{"s":"1748517658","n":"192000"},"h":"RTISP-10036","i":"0.0.0.0","a":"RTI Secure DDS Application","p":"85264","k":"50331706","x":[{"DDS":[{"domain_id":"0"},{"guid":"831AB06E.43876C36.FD825600.000001C1"},{"plugin_class":"DDS:Access:Permissions"},{"plugin_method":"RTI_Security_AccessControl_validate_status"}]}],"m":"permissions' validity period is invalid."}}
ERROR [0x831AB06E,0x43876C36,0xFD825600:0x000001C1|ADVANCE NOTIFY INVALID LOCAL PERMISSIONS|CHECK STATUS|LC:Security] PRESParticipant_onSecurityLocalCredentialValidateEvent:FAILED TO VALIDATE | Local permissions credentials.
ERROR [0x831AB06E,0x43876C36,0xFD825600:0x000001C1|ADVANCE NOTIFY INVALID LOCAL PERMISSIONS|LC:Security] PRESParticipant_onSecurityLocalCredentialEventListener:FAILED TO VALIDATE | Local credentials.

# Subscriber
ERROR [PARSE MESSAGE|0xDED844B7,0x87B9550F,0xB66DD964:0x000201C4{Entity=DR,MessageKind=DATA}|RECEIVE FROM 0x831AB06E,0x43876C36,0xFD825600:0x000201C3|:0x000001C1{Domain=0}|RECEIVE SAMPLE|PROCESS HANDSHAKE|GET SECURITY STATE|LC:Security] RTI_Security_PermissionsGrant_isValidTime:{"DDS:Security:LogTopicV2":{"f":"10","s":"3","t":{"s":"1748517682","n":"984966998"},"h":"RTISP-10036","i":"0.0.0.0","a":"RTI Secure DDS Application","p":"85248","k":"50331706","x":[{"DDS":[{"domain_id":"0"},{"guid":"DED844B7.87B9550F.B66DD964.000001C1"},{"plugin_class":"DDS:Access:Permissions"},{"plugin_method":"RTI_Security_PermissionsGrant_isValidTime"}]}],"m":"now is after not_after of permissions file"}}
ERROR [PARSE MESSAGE|0xDED844B7,0x87B9550F,0xB66DD964:0x000201C4{Entity=DR,MessageKind=DATA}|RECEIVE FROM 0x831AB06E,0x43876C36,0xFD825600:0x000201C3|:0x000001C1{Domain=0}|RECEIVE SAMPLE|PROCESS HANDSHAKE|GET SECURITY STATE|LC:Security] RTI_Security_AccessControl_validatePermissionsDocument:{"DDS:Security:LogTopicV2":{"f":"10","s":"3","t":{"s":"1748517682","n":"985028998"},"h":"RTISP-10036","i":"0.0.0.0","a":"RTI Secure DDS Application","p":"85248","k":"50331706","x":[{"DDS":[{"domain_id":"0"},{"guid":"DED844B7.87B9550F.B66DD964.000001C1"},{"plugin_class":"DDS:Access:Permissions"},{"plugin_method":"RTI_Security_AccessControl_validatePermissionsDocument"}]}],"m":"grant has invalid time"}}
ERROR [PARSE MESSAGE|0xDED844B7,0x87B9550F,0xB66DD964:0x000201C4{Entity=DR,MessageKind=DATA}|RECEIVE FROM 0x831AB06E,0x43876C36,0xFD825600:0x000201C3|:0x000001C1{Domain=0}|RECEIVE SAMPLE|PROCESS HANDSHAKE|GET SECURITY STATE|LC:Security] RTI_Security_AccessControl_validate_remote_permissions:{"DDS:Security:LogTopicV2":{"f":"10","s":"1","t":{"s":"1748517682","n":"985044998"},"h":"RTISP-10036","i":"0.0.0.0","a":"RTI Secure DDS Application","p":"85248","k":"50331706","x":[{"DDS":[{"domain_id":"0"},{"guid":"DED844B7.87B9550F.B66DD964.000001C1"},{"plugin_class":"DDS:Access:Permissions"},{"plugin_method":"RTI_Security_AccessControl_validate_remote_permissions"}]}],"m":"failed to validate remote permissions"}}
ERROR [PARSE MESSAGE|0xDED844B7,0x87B9550F,0xB66DD964:0x000201C4{Entity=DR,MessageKind=DATA}|RECEIVE FROM 0x831AB06E,0x43876C36,0xFD825600:0x000201C3|:0x000001C1{Domain=0}|RECEIVE SAMPLE|PROCESS HANDSHAKE|GET SECURITY STATE|LC:Security] DDS_DomainParticipantTrustPlugins_forwardGetAuthenticatedRemoteParticipantSecurityState:FAILED TO VALIDATE | Remote permissions.
ERROR [PARSE MESSAGE|0xDED844B7,0x87B9550F,0xB66DD964:0x000201C4{Entity=DR,MessageKind=DATA}|RECEIVE FROM 0x831AB06E,0x43876C36,0xFD825600:0x000201C3|:0x000001C1{Domain=0}|RECEIVE SAMPLE|PROCESS HANDSHAKE|LC:Security] PRESParticipant_authorizeRemoteParticipant:{"DDS:Security:LogTopicV2":{"f":"10","s":"3","t":{"s":"1748517682","n":"985078998"},"h":"RTISP-10036","i":"0.0.0.0","a":"RTI Secure DDS Application","p":"85248","k":"50331706","x":[{"DDS":[{"domain_id":"0"},{"guid":"DED844B7.87B9550F.B66DD964.000001C1"},{"plugin_class":"RTI:Auth"},{"plugin_method":"PRESParticipant_authorizeRemoteParticipant"}]}],"m":"unauthorized remote participant 831ab06e.43876c36.fd825600 denied by local participant ded844b7.87b9550f.b66dd964"}}
ERROR [PARSE MESSAGE|0xDED844B7,0x87B9550F,0xB66DD964:0x000201C4{Entity=DR,MessageKind=DATA}|RECEIVE FROM 0x831AB06E,0x43876C36,0xFD825600:0x000201C3|:0x000001C1{Domain=0}|RECEIVE SAMPLE|PROCESS HANDSHAKE|LC:Security] PRESParticipant_processHandshake:FAILED TO VALIDATE | Failed to authorize remote DP (GUID: 0x831AB06E,0x43876C36,0xFD825600:0x000001C1).
```

Communication will stop.

## Renewing the Permissions Document

This example updates the publisher DomainParticipant's Permissions Document
after 70 samples. At that point, communication with the subscriber will
resume.
