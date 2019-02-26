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

Build the example code by running the following command::

    mkdir build
    cd build
    cmake -DCONNEXTDDS_DIR=<connext dir> -DCONNEXTDDS_ARCH=<connext architecture> ..
    cmake --build .

This will produce a binary directory (*build*) where the ``Requester`` 
application can be found. The XML files in the source directory are also copied
over to this binary directory so that |RS| can be run directly from this 
directory as well.

**Note**: in order to build, ``CMake`` will invoke *RTI DDS Code Generator* on
the ``ServiceCommon.idl``, ``ServiceAdmin.idl`` and ``RecordingServiceTypes.idl``
files that can be found in ``$(CONNEXTDDS_DIR)/resource/idl`` directory. These
files define the communication interface for the remote administration platform
and any |RS|-specific types (``RecordingServiceTypes.idl``). The resulting 
generated C++ code will be stored in a folder called ``generated`` in
the CMake build directory.

Running the Example
===================

First of all, we need a running |RS| instance with remote administration enabled.
 

Requirements
============

To run this example you will need:

- RTI Connext Professional version 6.0.0 or higher.
- CMake version 3.10 or higher
