# Example Code: Using a Certificate Revocation List

## Building the Example

Remember to set your environment variables with the script in your Connext installation directory before building.

```sh
cd ../../c++11/
mkdir build
cd build
cmake ..
cmake --build .
```

Note: The build process also copies USER_QOS_PROFILES.xml into the build directory to ensure that it is loaded when you run the examples within the build directory.

## Setting up Security artifacts

The build process copies the security folder with .cnf files into the build directory. Use the provided python script to initialize them. This means that `build/security` will contain all the security artifacts needed to run this example.

```sh
cd security
python3 setup.py
```

Initializing an empty CRL

```sh
cd ca
python3 create_empty_crl.py
```

## Running the example

Demo is based on a standard rtiddsgen publisher and subscriber example code. Revoking of the certificate is done via the script in the ca folder:

Run a publisher and a subscriber in separate terminal windows.

```sh
./Crl_publisher
```

```sh
./Crl_subscriber
```

Verify that they communicate and that the subscriber is receiving data.

Run the revoke.py script from the `build/security/ca` directory in a separate terminal window, and communication between the participants will stop.

```sh
cd security/ca
python3 revoke.py
```

You will see the following on the subscriber:

```sh
[value: 53]
::ShapeType subscriber sleeping up to 1 sec...
::ShapeType subscriber sleeping up to 1 sec...
[value: 54]
::ShapeType subscriber sleeping up to 1 sec...
ERROR LC:SEC| RTI_Security_CertHelper_verifyCert:{"DDS:Security:LogTopicV2":{"f":"10","s":"3","t":{"s":"1699564657","n":"887532998"},"h":"RTI-10833","i":"0.0.0.0","a":"RTI Secure DDS Application","p":"191818","k":"security","x":[{"DDS":[{"domain_id":"0"},{"guid":"80a37417.1be0511e.e59657b5.1c1"},{"plugin_class":"Common"},{"plugin_method":"RTI_Security_CertHelper_verifyCert"}]}],"m":"X509_verify_cert returned 0 with error 23: certificate revoked
subject name: /C=US/ST=CA/L=Santa Clara/O=Real Time Innovations/emailAddress=ecdsa01ParticipantB@rti.com/CN=Crl Participant B
issuer name: /C=US/ST=CA/L=Santa Clara/O=Real Time Innovations/CN=RTI ECDSA01 (p256) ROOT CA/emailAddress=ecdsa01RootCa@rti.com"}}
ERROR LC:SEC| RTI_Security_Authentication_validateCertificateChain:{"DDS:Security:LogTopicV2":{"f":"10","s":"3","t":{"s":"1699564657","n":"887734998"},"h":"RTI-10833","i":"0.0.0.0","a":"RTI Secure DDS Application","p":"191818","k":"security","x":[{"DDS":[{"domain_id":"0"},{"guid":"80a37417.1be0511e.e59657b5.1c1"},{"plugin_class":"Authentication"},{"plugin_method":"RTI_Security_Authentication_validateCertificateChain"}]}],"m":"Identity verification failed. Make sure it was signed by the right authority."}}
ERROR LC:SEC| RTI_Security_CertHelper_logMessageForEveryCa:{"DDS:Security:LogTopicV2":{"f":"10","s":"3","t":{"s":"1699564657","n":"887828998"},"h":"RTI-10833","i":"0.0.0.0","a":"RTI Secure DDS Application","p":"191818","k":"security","x":[{"DDS":[{"domain_id":"0"},{"guid":"80a37417.1be0511e.e59657b5.1c1"},{"plugin_class":"Common"},{"plugin_method":"RTI_Security_CertHelper_logMessageForEveryCa"}]}],"m":"Failed to verify identity. Used authority: /C=US/ST=CA/L=Santa Clara/O=Real Time Innovations/CN=RTI ECDSA01 (p256) ROOT CA/emailAddress=ecdsa01RootCa@rti.com"}}
ERROR LC:SEC| PRESParticipant_revalidateLocalParticipantsSecureStatus:VALIDATION FAILURE | Local Identity Status. Identity certificate was revoked.
ERROR PRESParticipant_onSecureStatusChanged:VALIDATION FAILURE | Identity of local DomainParticipant
ERROR DDS_DomainParticipantTrustPlugins_on_status_changed:FAILURE | change security status
::ShapeType subscriber sleeping up to 1 sec...
```

It will no longer receive data. The publisher will also print a log message:

```sh
ERROR [0x94E003CE,0x56F44FC9,0x1A05CFC2:0x000001C1|VALIDATE REMOTE PARTICIPANT IDENTITY|CHECK AUTHENTICATION STATUS|LC:SEC]RTI_Security_CertHelper_verifyCert:{"DDS:Security:LogTopicV2":{"f":"10","s":"3","t":{"s":"1699564658","n":"528682999"},"h":"RTI-10833","i":"0.0.0.0","a":"RTI Secure DDS Application","p":"191602","k":"security","x":[{"DDS":[{"domain_id":"0"},{"guid":"94e003ce.56f44fc9.1a05cfc2.1c1"},{"plugin_class":"Common"},{"plugin_method":"RTI_Security_CertHelper_verifyCert"}]}],"m":"X509_verify_cert returned 0 with error 23: certificate revoked
subject name: /C=US/ST=CA/L=Santa Clara/O=Real Time Innovations/emailAddress=ecdsa01ParticipantB@rti.com/CN=Crl Participant B
issuer name: /C=US/ST=CA/L=Santa Clara/O=Real Time Innovations/CN=RTI ECDSA01 (p256) ROOT CA/emailAddress=ecdsa01RootCa@rti.com"}}
ERROR [0x94E003CE,0x56F44FC9,0x1A05CFC2:0x000001C1|VALIDATE REMOTE PARTICIPANT IDENTITY|CHECK AUTHENTICATION STATUS|LC:SEC]RTI_Security_Authentication_validateCertificateChain:{"DDS:Security:LogTopicV2":{"f":"10","s":"3","t":{"s":"1699564658","n":"528888999"},"h":"RTI-10833","i":"0.0.0.0","a":"RTI Secure DDS Application","p":"191602","k":"security","x":[{"DDS":[{"domain_id":"0"},{"guid":"94e003ce.56f44fc9.1a05cfc2.1c1"},{"plugin_class":"Authentication"},{"plugin_method":"RTI_Security_Authentication_validateCertificateChain"}]}],"m":"Identity verification failed. Make sure it was signed by the right authority."}}
ERROR [0x94E003CE,0x56F44FC9,0x1A05CFC2:0x000001C1|VALIDATE REMOTE PARTICIPANT IDENTITY|CHECK AUTHENTICATION STATUS|LC:SEC]RTI_Security_CertHelper_logMessageForEveryCa:{"DDS:Security:LogTopicV2":{"f":"10","s":"3","t":{"s":"1699564658","n":"528947999"},"h":"RTI-10833","i":"0.0.0.0","a":"RTI Secure DDS Application","p":"191602","k":"security","x":[{"DDS":[{"domain_id":"0"},{"guid":"94e003ce.56f44fc9.1a05cfc2.1c1"},{"plugin_class":"Common"},{"plugin_method":"RTI_Security_CertHelper_logMessageForEveryCa"}]}],"m":"Failed to verify identity. Used authority: /C=US/ST=CA/L=Santa Clara/O=Real Time Innovations/CN=RTI ECDSA01 (p256) ROOT CA/emailAddress=ecdsa01RootCa@rti.com"}}
```

## Simulating Certificate Renewal

To restart communication, call the script again to give the participants new certificates. You can do this without restarting the applications.

```sh
cd build/security
python3 setup.py
cd ca
python3 create_empty_crl.py
```

They will start communicating again, because of the `com.rti.serv.secure.authentication.identity_certificate_file_poll_period.millisec` property. This allows you to renew your certificates without having to restart your applications.
