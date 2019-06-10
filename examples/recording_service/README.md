# Recording Service

RTI Recording Service includes three tools:

-   Recording Service, an RTI Connext DDS application that records Topics and
    discovery data. Recording Service records updates to data along with a
    timestamp, so you can view or replay updates to data in your system as they
    occur over time. Recorded data is stored in SQLite® files by default.
    Recording Service also has an API to record to a custom data store.

-   Replay Service, an application that can play back data recorded by
    Recording Service. Replay Service also has an API to allow plugging in
    custom storage.

-   Converter, an application that converts binary (serialized) recorded data
    to deserialized data that can be viewed and queried. The most efficient way
    to record data is in serialized form. Converter allows data to be recorded
    efficiently, then post-processed into a queryable form. Converter also
    provides APIs to plug in custom storage.

In this directory you will find some examples about the Recording Service.
For more info check out the
[documentation](https://community.rti.com/static/documentation/connext-dds/6.0.0/doc/manuals/recording_service/introduction.html).
