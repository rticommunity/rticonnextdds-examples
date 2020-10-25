Example Code: MongoDB Adapter
=============================

Building
--------

In order to build this example, you need to provide the following variables to
``CMake``:

- ``CONNEXTDDS_DIR``: path to the connext installation root folder
- ``CONNEXTDDS_ARCH``: target architecture
- ``MONGODB_DIR``: path to the mongodbcxx installation root folder

.. code::

    mkdir build
    cmake -DCONNEXTDDS_DIR=<Connext DDS Directory> \
            -DCONNEXTDDS_ARCH=<Connext DDS Architecture> \
            -DMONGODB_DIR=<mongodbcxx directory> \
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
*RTI Connext DDS* installer bundle. We provide a single ``RoutingService``
configuration that defines entities to forward incoming DDS traffic from any Topic
to a configurable MongoDB database.



For reusability of the configuration, we define the following XML configuration variables:

- ``CLUSTER_ADDRESS``: cluster address where the MongoDB instance runs. The default value
  points to the pre-onfigured public cluster.
- ``DB_NAME``: name of the database. The default value points to the default database in
  the preconfigured cluster.
- ``USER_AND_PASS``: user name and password, specified as ``<user>:<pass>``. No default
  value provided in XML, so you must provide it via command-line (or through the equivalent
  method if using the Service API.

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
            -domainId=<your_domain>

You can then run `ShapesDemo` on the selected domain and publish ``Squares``,
``Circles``, and ``Triangles``. You can start `ShapesDemo` from the command line
and select the domain and publication rate to one second as follows:

.. code::

    $<Connext DDS Directory>/bin/rtishapesdemo -domainId <your_domain> -pubInterval 1000

You can then connect to the ``MongoDB`` using the shell:

.. code::

    mongo "mongodb+srv://cluster0.8u6dg.mongodb.net/<dbname>" --username rti_example

You will then be prompted to introduce the user password (above mentioned). The you
can inspect the items that have been created. You can run the following commands
to select the database, see the collections (Topics), and documents inserted:


.. code::

    MongoDB Enterprise atlas-bo2ggf-shard-0:PRIMARY> use dds_space
    MongoDB Enterprise atlas-bo2ggf-shard-0:PRIMARY> show collections
    Circle
    Square
    Triangle
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



Requirements
------------

To run this example you will need:

- RTI Connext Professional version 6.0.0 or higher.
- CMake version 3.10 or higher.
- A target platform with support for RTI Routing Service and C++11.
- A running MongoDB cluster or local database
- MongoDB shell.
