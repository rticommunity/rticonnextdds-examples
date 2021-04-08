Example Code: MongoDB Adapter
=============================

Building
--------

In order to build this example, you need to provide the following variables to
``CMake``:

- ``MONGODB_DIR``: path to the ``mongodbcxx`` installation root folder
- ``CMAKE_BUILD_TYPE``: specifies the build mode. Valid values are Release and 
  Debug
- ``BUILD_SHARED_LIBS``: whether to build the library as a shared library or not.
  This should be set to ON when loading the plugin from the Routing Service
  executable. However, Routing Service supports loading of static libraries when
  using the service as a library. See note below.

.. code::

    mkdir build
    cmake -DMONGODB_DIR=<mongodbcxx directory> \
            -DBUILD_SHARED_LIBS=ON \
            -DCMAKE_BUILD_TYPE=Release ..
    cmake --build .


.. note::

    If you are using a multi-configuration generator, such as Visual Studio
    Solutions, you can specify the configuration mode to build as follows:

    .. code::

        cmake --build . --config Release|Debug


Here is more information about generating
`Visual Studio Solutions for Windows using CMake <https://cmake.org/cmake/help/v3.16/generator/Visual%20Studio%2016%202019.html#platform-selection>`_.

.. note::

    The ``BUILD_SHARED_LIBS`` allows you to control if the generated library for this
    example is a static or a dynamic shared library. The following sections
    assume you are building a dynamic shared library. However Routing Service also
    supports static linking of adapters. To use this functionality you would need to
    create an application that uses Routing Service as a library component and
    statically link to this adapter.

Configuring Connext DDS Installation Path and Architecture
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The CMake build infrastructure will try to guess the location of your Connext
DDS installation and the Connext DDS architecture based on the default settings
for your host platform. If you installed Connext DDS in a custom location, you
can use the `CONNEXTDDS_DIR` variable to indicate the path to your RTI Connext 
DDS installation folder. For example:

.. code::

    cmake -DCONNEXTDDS_DIR=/home/rti/rti_connext_dds-x.y.z ...

Also, If you installed libraries for multiple target architecture on your system
(i.e., you installed more than one target rtipkg), you can use the
`CONNEXTDDS_ARCH` CMake variable to indicate the architecture of the specific 
libraries you want to link against. For example:

.. code::

    cmake -DCONNEXTDDS_ARCH=x64Linux3gcc5.4.0 ...


Running
-------

To run the example, you just need to run the following command from the ``build``
folder (where the adapter plugin library has been created). You will also need a running
instance of a MongoDB database. We have created a free account with a single cluster
that you can reuse to run this example:

- address: ``cluster0.8u6dg.mongodb.net``
- default database: ``dds_space``
- user and pass: ``rti_example:adapter``

This example can work with any DDS topic and we will use RTI Shapes Demo, shipped with
*RTI Connext Professional* installer bundle. We provide a single ``RoutingService``
that acts as a *gateway* between DDS and MongoDB. This configuration allows simultaneous
two-way communication between DDS and MongoDB, considering two separate DDS domains to
avoid feedback loops:

- An input DDS domain from where data from each Topic is inserted to the corresponding
  database collection.
- An output DDS domain to where data from MongoDB is sent, demultiplexing each collection
  into its corresponding Topic.

The domain IDs for these domain are 0 and 1, respectively. This is done for convenience
so that we can later on provide the ``-domainIdBase`` as the ID offset in order to select
different domain ID values.

When you run this configuration, you will observe an echo-like effect. The samples
generated from the input DDS domain are first stored in MongoDB. This data flow is
provided by the ``fromDdsToMongo`` *AutoRoute*, which generate routes with an input
*DataReader* and an output *StreamWriter* from the ``MongoConnection``. Alternatively,
the ``fromMongoToDds`` *AutoRoute* generates routes with an input *StreamReader* from
the ``MongoConnection`` and an output *DataWriter*. Both *AutoRoutes* are configured
with the default *Processor* that simply forwards all the data from the inputs to the
outputs.

For reusability of the configuration, we define the following XML configuration variables:

- ``CLUSTER_ADDRESS``: address where the MongoDB instance runs. The default value
  points to the pre-configured public cluster.
- ``DB_NAME``: name of the database. The default value points to the default database in
  the preconfigured cluster.
- ``USER_AND_PASS``: user name and password, specified as ``<user>:<pass>``. No default
  value provided in XML, so you must always provide it.

To run Routing Service, you will need first to set up your environment as follows:

.. code::

    $export RTI_LD_LIBRARY_PATH=<Connext DDS Directory>/lib/<Connext DDS Architecture>

.. note::

    If you wish to run the executable for a target architecture different
    than the host, you have to specify your architecture. That way the Routing 
    Service script can use the specific target binary instead of using the standard 
    host binary. This can be done by using the ``CONNEXTDDS_ARCH`` variable.

    .. code::

    $export CONNEXTDDS_ARCH=<Connext DDS Architecture>


And then you can run the following command, providing the domain ID of your choice:

.. code::

    # From the build/ directory
    $<Connext DDS Directory>/bin/rtiroutingservice \
            -cfgFile RsMongoGateway.xml \
            -cfgName MongoGateway \
            -DUSER_AND_PASS=rti_example:adapter
            -domainIdBase=<domain_id_offset>

Run a publication `ShapesDemo` on the selected domain (0 + ``domain_id_offset``) and
publish ``Squares``, ``Circles``, and ``Triangles``. You can start `ShapesDemo` from the
command line and select the domain and publication rate to one second as follows:

.. code::

    $<Connext DDS Directory>/bin/rtishapesdemo -domainId <input_domain> -pubInterval 1000

You can then connect to the ``MongoDB`` using the shell:

.. code::

    mongo "mongodb+srv://cluster0.8u6dg.mongodb.net/<dbname>" --username rti_example

You will be prompted to introduce the user password (above mentioned). Within the shell,
can inspect the items that have been created. For example, run the following commands
to select the database, see the collections (Topics), and documents inserted:

.. code::

    # Select database
    MongoDB Enterprise atlas-bo2ggf-shard-0:PRIMARY> use dds_space

    # List current collections
    MongoDB Enterprise atlas-bo2ggf-shard-0:PRIMARY> show collections
    Circle
    Square
    Triangle

    # Display all data in a collection
    MongoDB Enterprise atlas-bo2ggf-shard-0:PRIMARY> db.Square.find({}).pretty()
    {
	"_id" : ObjectId("5f95fc093c475a08726ab5b5"),
	"data" : {
		"color" : "BLUE",
		"x" : 178,
		"y" : 164,
		"shapesize" : 30,
		"fillKind" : NumberLong(0),
		"angle" : 0
	},
	"info" : {
		"instance_handle" : BinData(5,"ysIXwxg2P47xFg7u3vnohg=="),
		"source_timestamp" : {
			"sec" : 1603664905,
			"nanosec" : NumberLong(318101000)
		},
		"reception_timestamp" : {
			"sec" : 1603664905,
			"nanosec" : NumberLong(318184000)
		},
		"original_publication_virtual_guid" : BinData(5,"AQGwLjJJeAhIHGucgAAAAg=="),
		"original_publication_virtual_sequence_number" : {
			"high" : 0,
			"low" : NumberLong(29)
		},
		"related_original_publication_virtual_guid" : BinData(5,"AAAAAAAAAAAAAAAAAAAAAA=="),
		"related_original_publication_virtual_sequence_number" : {
			"high" : -1,
			"low" : NumberLong("4294967295")
		},
		"topic_query_guid" : BinData(5,"AAAAAAAAAAAAAAAAAAAAAA==")
	}
    }
    ...

To verify the downlink communication, run a subscription `ShapesDemo` on the selected
domain (1 + ``domain_id_offset``) and subscribe to ``Squares``, ``Circles``, and
``Triangles``. You can start `ShapesDemo` from the command line and select the domain as
follows:

.. code::

    $<Connext DDS Directory>/bin/rtishapesdemo -domainId <output_domain>

You should see data being displayed at the polling period specified in the
``fromMongoToDds``. Note that this *AutoRoute* has a ``<periodic_action>`` tag set, which
establishes the rate at which samples are read from the database. This required since
this adapter implementation relies on a polling mechanism from *RoutingService*.

Requirements
------------

To run this example you will need:

- RTI Connext Professional version 6.1.0 or higher.
- CMake version 3.11 or higher.
- A target platform with support for RTI Routing Service and C++11.
- A running MongoDB cluster or local database
- MongoDB C++ driver v3 installed in the build machine.
- MongoDB shell.
