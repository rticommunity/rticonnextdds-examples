Example Code: MongoDB Adapter
=============================

Building
--------

In order to build this example, you need to define the variables ``CONNEXTDDS_DIR``
and ``CONNEXTDDS_ARCH``. You can do so by exporting them manually, by sourcing 
the ``rtisetenv`` script for your architecture, or by passing them to the ``cmake``
command as arguments. 

You also need to define the variable ``MONGODB_DIR`` as the path to the
``mongodbcxx`` installation directory, which can be either exported or passed as
an argument to ``cmake``, but won't be set by ``rtisetenv``.

.. code::
    mkdir build
    cd build
    cmake -DCONNEXTDDS_DIR=<Connext DDS Directory> \     # If not exported
          -DCONNEXTDDS_ARCH=<Connext DDS Architecture> \ # If not exported
          -DMONGODB_DIR=<mongodbcxx directory> \         # If not exported
          -DBUILD_SHARED_LIBS=ON|OFF \
          -DCMAKE_BUILD_TYPE=Debug|Release ..
    cmake --build .

.. note::

   You do not need to define ``CONNEXTDDS_ARCH`` if you only have one architecture
   target installed in your system.

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
