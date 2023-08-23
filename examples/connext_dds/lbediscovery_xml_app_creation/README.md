# Example Code: LBED and XML-Based Application Creation

## Concept

The *RTI Limited Bandwidth Endpoint Discovery (LBED) Plugin* reduces discovery
time and network traffic by locally defining information about the endpoints
that need to be discovered in an XML file. The Simple Endpoint Discovery Protocol
(SEDP), the default dynamic endpoint discovery process, on the other hand, sends
the information about the endpoints over the network. Therefore, LBED requires all
the endpoints to be known ahead of time and each must be declared in an XML file.

One of the requirements for using LBED in an application is that every endpoint
must define a value for the RTPS object ID. The RTPS object ID is an integer
that uniquely identifies an endpoint of a specific kind (DataWriter or DataReader)
within a DomainParticipant. Normally, the RTPS object ID is automatically assigned
by Connext when an endpoint is created, and it is exchanged during the Simple
Endpoint Discovery phase. However, the LBED Plugin replaces Simple Endpoint
Discovery, so this auto-assigned RTPS object ID will not be propagated to other
endpoints. In order for remote endpoints to be discovered, LBED needs to know
the RTPS object ID of a remote participant's entities before discovery initiates.

If your application uses XML-Based Application Creation, however, you do not have
to manually specify the RTPS object ID. In this unique case, LBED is able to
automatically infer which RTPS object ID Connext will assign to each endpoint.

## Example description

This example shows how to configure LBED when your application uses XML-Based
Application Creation. You will learn that, in this specific case, you don't need
to define an RTPS object ID for every endpoint, and that you can use the DDS-XML
`multiplicity` attribute for creating several instances of the same endpoint.

Keep in mind that LBED is configured exclusively through XML. The source code
is just an example application using XML-Based Application Creation.

The example uses the `ShapeType` from *RTI Shapes Demo*.

## Publisher

The Publisher application has two `ShapeType` DataWriters, both in the
`ShapeTypeTopic`. One of them publishes samples with `color=RED`. The other one,
samples with `color=BLUE`.

Both are instances of the same DataWriter: they are created using the DDS-XML
`multiplicity` attribute. Notice that the *"ShapeType_PublisherProfile"*
(in the USER_QOS_PROFILES.xml) doesn't specify an `rtps_object_id` for these
DataWriters.

To finalize the Publisher, either set the `--sample-count` option or send
a SIGINT/SIGTERM signal. The application will finish gracefully.

## Subscriber

The Subscriber application has four `ShapeType` DataReaders, all in the
`ShapeTypeTopic`. Two of them have a `ContentFilterTopic` that filters samples
with `color=RED`. The other two filter samples with `color=BLUE`.

Each pair of endpoints are instances of the same two DataReaders: they are
created using the DDS-XML `multiplicity` attribute. Notice that the
*"ShapeType_SubscriberProfile"* (in the USER_QOS_PROFILES.xml) doesn't specify
an `rtps_object_id` for these DataReaders.

To finalize the Subscriber, either set the `--sample-count` option or send
a SIGINT/SIGTERM signal. The application will finish gracefully.
