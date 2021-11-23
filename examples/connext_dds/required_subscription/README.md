# Example Code: Required Subscription

## Concept

There are scenarios where you know a priori that a particular set of applications
will join the system: e.g., a logging service or a known processing application.

The *Required Subscription* feature is designed to keep data until these known late
joining applications acknowledge the data.

The *Required Subscription* feature will also keep the data until known DataReaders
which become inactive or removed from discovery databases have acknowledged the data.

## Example Description

This example has two actors:

-   The subscriber application that receives the samples and is configured to use
    the *Required Subscription* feature.
-   The publisher application that writes only one sample and calls
    *wait_for_acknowledgments()* before finishing. So it will wait until the
    subscriber is executed and, after it, it will finish.

*USER_QOS_PROFILES.xml* has been modified as follows:

-   Durability set to *TRANSIENT_LOCAL_DURABILITY_QOS*. This is mandatory for the
    DataWriter, otherwise a runtime error will occur. This is not stricty mandatory
    for the DataReader but, in this case, it will not behave as a *Required Subscriber*
    and the DataWriter will never finish.

-   Set *publication_name* and *subscription_name*, both name and role.

-   Add the *Availability QoS* in the DataWriter. Note that the *role_name* that
    is in *required_matched_endpoint_groups* must match the DataReader *role_name*
    defined in *subscription_name*. Otherwise, the publisher application will
    never finish.
