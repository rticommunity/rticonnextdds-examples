Example Code: MongoDB Adapter
==============================

Concept
-------

This example leverages the `RTI Routing Service Adapter SDK` to integrate DDS with
`MongoDB`. It shows how to build an adapter that uses the MongoDB C++ Driver to
perform operations in a running instance of a MongoDB database.

.. figure:: images/RouterMongoDBExample.svg
    :figwidth: 40%
    :align: center

This example assumes that you are familiar with both `Routing Service` and `MongoDB`.
You can find the reference documentation for both products following the links below:

- `RTI Routing Service User's Manual (version 7.0.0) <https://community.rti.com/static/documentation/connext-dds/7.0.0/doc/manuals/connext_dds_professional/services/routing_service/index.html>`_
- `Mongo DB Manual (current version) <https://docs.mongodb.com/manual/>`_
- `Mongo DB C++ Driver (v3) <http://mongocxx.org/mongocxx-v3/>`_


Example Description
-------------------

The example provides two main components:

- A `RoutingService` adapter library that links with the ``MongoDB`` C++ driver library
  (``mongocxx``).

- A `RoutingService` configuration that instantiates this example adapter together with
  a DDS `Participant`, and establishes an automatic route to integrate user `Topic` data
  with a ``MongoDB`` database.

You will see how with `RoutingService` you can get data from a DDS domain into a `MongoDB`
database that runs in a cloud-based cluster. The sections below provide details about
the adapter architecture and the mapping between DDS and MongoDB concepts.

DDS Mapping
-----------

The mapping between DDS and MongoDB selected in this example consists of the following
items:

- A DDS `domain` maps into a single `Database`, whose name is arbitrary and user provided.
  Domain in this context does not reflect a single domain ID but instead a cohesive
  system domain, which may be comprised of one or more domain IDs.

- A DDS `Topic` maps into a single database `Collection`, whose name is the actual
  name of the `Topic`.

- `Topic` samples map into individual `Document` objects. An object for a DDS sample
  always contains two top-level members: a ``data`` item that contains the user data (whose
  content depends on the actual user-defined type) and an ``info`` item that contains the
  associated metadata (``SampleInfo``).

Adapter Architecture
--------------------

Generally speaking, applications that access `MongoDB` database using the driver API
require performing the following operations:

- Create a client connection to a concrete database running instance.
- Obtain a handle to the database.
- Use the handle to perform operations (read, write, etc) to the database.

With this procedure in mind and considering some of the restrictions integrating the
driver in the application–such as concurrency constraints-, we propose the following model
for the adapter:

.. figure:: images/RouterMongoDBAdapterClass.svg
    :figwidth: 70 %

- ``AdaterPlugin``: keeps a reference to the ``mongocxx::instance``, which is the
  singleton unit for the driver. This is semantically equivalent to the plugin instance,
  which is expected to be loaded only once by a `Routing Service` instance.
- ``Connection``: instantiates a single client pool and identifies a single database. This
  model allows multiple ``StreamWriters`` and ``StreamReaders`` to obtain clients from
  the pool as needed. This is important to support concurrency with multiple threads
  within a session.
- ``StreamWriter``: uses the parent ``Connection`` to obtain a client database handle
  and insert `Document` objects in the write operation. The ``StreamWriter`` uses the
  ``SampleConverter`` helper functions to convert ``DynamicData`` and ``Info`` samples
  to ``MongoDB`` objects.
- ``StreamReader``: uses the parent ``Connection`` to obtain a client database handle
  and read ``Document`` objects in the read operation. The ``StreamReader`` uses the
  ``SampleConverter`` helper functions to convert ``MongoDB`` document objects into
  ``DynamicData``. ``Info`` samples are not converted and it relies on the output
  adapter to generate the proper metadata based on the provided data. An important
  detail to consider is that the ``StreamReader`` does not provide asynchronous event
  notifications of data arrival. This means that a periodic event must be set in the
  parent ``Route`` or ``AutoRoute`` as the polling period.

The source code contains additional documentation with more implementation details.

