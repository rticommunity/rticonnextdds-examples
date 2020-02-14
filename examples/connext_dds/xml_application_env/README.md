# Example Code: XML Application Creation - Environment

## Concept

The purpose of this example is to demonstrate the use of XML Application Creation
and the DynamicData API.  The use case is to have an application publish
environment variables for itself as well as receive environment variables
from other instances.  These are published to the XML defined EnvironmentTopic.

All the DDS entities are described in a single XML file using
[Connext DDS XML Application Creation](https://community.rti.com/static/documentation/connext-dds/current/doc/manuals/connext_dds/xml_application_creation/html_files/RTI_ConnextDDS_CoreLibraries_XML_AppCreation_GettingStarted/index.htm)
In code we make use of these entities by loading the file with the QosProvider
and looking up a individual entity by name.

In this example both the publisher and subscriber are defined in the same
participant.  By default, this means that when we publish a sample with the
writer, we will receive it on our reader.  While this may be desireable in some
cases, in this case we specifically ignore our own published samples so we only
receive data from other publishers, not our own.

## Example Description

The code for this example is laid out in the following way:

1)  XML file *application.xml* (also known as a QoS file) is defined using the
[RTI DDS Profiles Schema](http://community.rti.com/schema/6.0.0/rti_dds_profiles.xsd).
This file defines the type ```KeyValuePair```, the topic ```EnvironmentTopic```
as well as the ```Participant``` and all its child publisher, subscriber, 
reader and writer.
           
2)  The participant is created using the call to ```create_participant_from_config()```.
Using the participant the reader and writer are found and stored as references
in the code.  The reader and the writer are both specialized as <DynamicData> which
allows the XML defintion of the type to be used without any code generation step.

3)  A listener is registered on the reader.  When the ```on_data_available()``` event
handler is called the list of available samples is traversed and valid samples are
printed to the console.

4)  ```ignore()``` is called for the participant to ensure that we
do not receive samples that we publish from our own participant.  This allows
an application to publish and subscribe to a topic and not have to receive
data that was published by itself.

5)  A random number is generated as an application id.  This value is used as a key
in the EnvironmentTopic to allow multiple instances of the application to be
publishing their data independently.

6)  The main loop repeately publishes the environment variables.  The array of
variables are provided to the main function in the ```char *envp[]``` parameter.
