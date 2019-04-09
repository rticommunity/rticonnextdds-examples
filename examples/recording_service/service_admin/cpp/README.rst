Example Code: Recording Service Administration
**********************************************

.. |RS| replace:: *Recording Service*

Concept
=======

|RS| uses the common Remote Administration Platform (see
`Remote Administration Platform <https://community.rti.com/static/documentation/connext-dds/6.0.0/doc/manuals/recording_service/common/remote_admin_platform.html>`_).
This example shows how to use the platform to send remote commands to a running
instance of Recording Service. Note that this application can also work with a
running instance of *RTI Routing Service*, as the admin platform is common to
both).

Example Description
===================

This example shows a one-shot application that can be used to send one remote
command to a running application of Recording Service (you can also try to use
it with Routing Service). The application will prepare a command request based
on the command-line parameters provided in the application invocation. After
sending that request out to the service, it will wait for a command reply to be
sent by the service. The application will then exit.

The following files are part of the example:

- ``Requester.hpp`` and ``Requester.cxx``: these files contain the example C++
  code that prepares and sends the command request and then waits for a reply
  from the running service instance.

- ``USER_QOS_PROFILES.xml``: this file defines DDS XML QoS profiles to be used
  by the application and by *Recording Service*.

- ``recorder_remote_admin.xml``: this is a configuration file for Recording
  Service that enables remote administration, and uses the QoS profiles defined
  in the XML QoS profiles file defined above. By using the same QoS profiles in
  both the application and the *Recording Service* we ensure the QoS matching 
  and the reliability of the communication.

Building the Example
====================

In order to build this example, you need to provide the following variables to
``CMake``:

- ``CONNEXTDDS_DIR``: pointing to the installation of RTI Connext DDS to be 
  used.

- ``CONNEXTDDS_ARCH``: the RTI Connext DDS Target architecture to be used in 
  your system.

Build the example code by running the following command:

.. code-block:: text

    mkdir build
    cd build
    cmake -DCONNEXTDDS_DIR=<connext dir> -DCONNEXTDDS_ARCH=<connext architecture> ..
    cmake --build .

This will produce a binary directory (*build*) where the ``Requester`` 
application can be found. The XML files in the source directory are also copied
over to this binary directory so that |RS| can be run directly from this 
directory as well.

.. note::
    In order to build, ``CMake`` will invoke *RTI DDS Code Generator* on
    the ``ServiceCommon.idl``, ``ServiceAdmin.idl`` and ``RecordingServiceTypes.idl``
    files that can be found in ``$(CONNEXTDDS_DIR)/resource/idl`` directory. These
    files define the communication interface for the remote administration platform
    and any |RS|-specific types (``RecordingServiceTypes.idl``). The resulting 
    generated C++ code will be stored in a folder called ``generated`` in
    the CMake build directory.

.. note::
    **Cross-compilation**. When you need to cross-compile the example, the above
    command will not work, the assigned compiler won't be the cross-compiler and
    errors may happen when linking against the cross-compiled Connext binaries.
    To fix this, you have to create a file with the architecture name and call 
    CMake with a specific flag called ``-DCMAKE_TOOLCHAIN_FILE``.
    An example of the file to create with the toolchain settings (e.g. for an 
    ARM architectures):

.. code-block:: cmake

    set(CMAKE_SYSTEM_NAME Linux)
    set(toolchain_path "<path to>/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian")
    set(CMAKE_C_COMPILER "${toolchain_path}/bin/arm-linux-gnueabihf-gcc")
    set(CMAKE_CXX_COMPILER "${toolchain_path}/bin/arm-linux-gnueabihf-g++")

Then you can call CMake like this:

.. code-block:: text

    cmake -DCONNEXTDDS_DIR=<connext dir> -DCMAKE_TOOLCHAIN_FILE=<toolchain file created above> 
            -DCONNEXTDDS_ARCH=<connext architecture> ..

Running the Example
===================

First of all, we need a running |RS| instance with remote administration enabled.
We will use the |RS| configuration shipped with the example, called 
``recorder_remote_admin.xml``:

.. code-block:: text

    cd <binary directory>
    $(CONNEXTDDS_DIR)/bin/rtirecordingservice 
            -cfgFile recorder_remote_admin.xml -cfgName remote_admin

Note: you can run from other directory, just make sure the 
``recorder_remote_admin.xml`` and ``USER_QOS_PROFILES.xml``

Once the |RS| instance is running, it is ready to receive remote command requests.
The administration app receives the following command-line parameters:

- The kind of command to be sent, following a *CRUD* pattern (CREATE, READ, 
  UPDATE, DELETE).
- The identifier of the resource we're accessing, 
  e.g. ``recording_services/remote_admin/state``
- [Optional] The arguments that the command kind needs for performing the action,
  e.g. ``paused`` (if the objective state of the service is *paused*)
- [Optional] ``--domain-id``: this parameter should be used when the |RS| 
  instance is running admninistration on a DDS domain different than 0 (this can
  be achieved by modifying the XML configuration or using the 
  ``-remoteAdministrationDomainId`` parameter in the invocation). The domain
  specified for this command-line parameter and the one used for remote 
  administration of the service should be the same, otherwise the application
  won't be able to connect to the service.
- [Optional] ``--time-tag``: there are commands that are service-specific. This
  command-line parameter allows the user to define a timestamp tag for |RS| to
  store. These tags can later be used to easily, symbolically define time ranges
  to be replayed or converted. This command requires a name parameter and 
  optionally, a description (remember to use quotes for multi-word descriptions).

To run the administration app, you should go to the binary directory, or if you
run from other directories, make sure the ``USER_QOS_PROFILES.xml`` file is
present in the directory. We'll now show you some examples on how to send
different commands to the service.

1. Sending a pause command:

.. code-block:: text

    Requester UPDATE /recording_services/remote_admin/state paused

2. Sending a timestamp tag command to a service with administration running on
DDS domain ID 54:

.. code-block:: text

    Requester UPDATE /recording_services/remote_admin/storage/sqlite:tag_timestamp --domain-id 54 --time-tag "A timestamp tag" "A timestamp tag description"

3. Sending a shutdown command

.. code-block:: text

    Requester DELETE /recording_services/remote_admin
 

Requirements
============

To run this example you will need:

- RTI Connext Professional version 6.0.0 or higher.
- CMake version 3.11 or higher
