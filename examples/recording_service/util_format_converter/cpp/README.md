Recording Service Storage plug-in: CSV Converter
************************************************

.. |RecS| replace:: *RecordingService*
.. |SP| replace:: *Storage plug-in*
.. |CSV| replace:: ``CSV``
.. |SD| replace:: *ShapesDemo*.

.. |br| raw:: html

   <br />

Introduction
============

This module contains the implementation of a |RecS| |SP| that writes data into
a text file in *comma-separated value* (|CSV|) format. The expected usage of
this plug-in is with the |RecS| *Converter* tool, to convert an existing
database from a previous recording into a set of text file(s) containing the
data in |CSV| format.

Requirements
------------

To use this plug-in you will need:

- RTI Connext Professional version 6.0.0 or higher.
- CMake version 3.5 or higher
- A target platform supported by *RTI* |RecS|.

Capabilities and Usage
======================

This plug-in stores the data provided to the *Output* |SP| in |CSV| format. By
default, the plug-in generates a separate |CSV| file for each *Topic*. The
content and format of each file is a follows:

.. list-table:: |CSV| file format
    :name: TableCsvFileFormat
    :widths: 15 20 10 10 10 10

    * - Row 1: Topic Entry
      - ``Topic name: <Name>``
      - 
      - 
      -       
      -
    * - Row 2: Type Header
      - ``timestamp``
      - ``member1``
      - ``member2``
      - ...
      - ``memberN``
    * - Row 3: Data values
      - Reception timestamp  in nanoseconds of the sample
      - Value for member1
      - Value for member2
      - ...
      - Value for member2

The name of the file is formed using the topic name and has ``.csv`` extension.
All the files are placed in a directory that can be specified in the
plug-in configuration.

Mapping of a data sample into columns
-------------------------------------

General case
^^^^^^^^^^^^

As shown in the table above, a data sample is represented in a single row
comprising  multiple columns. Each cell holds only a value for a *final* or
leaf member in a complex data type. That is, for a member whose type is
a *Simple* type (integer, char, short), enumeration, or *String*.

Each data value in a cell corresponds to a member whose name is in the
type header. Given a complex data type, the name for a member is constructed
as follows:

::

    .<parent_member1>.<parent_member2>...<parent_memberN>.<final_member>

where ``<parent_member>`` is the name of the parent complex member that
contains the subsequent member.

If either ``<parent_member>`` or ``<final_member>`` is a *Collection* type
(array or sequence), the member name is suffixed with ``[<index>]`` an a column
for each possible element in the *Collection* is created.

Example
'''''''

Consider the following type described in IDL:

::

    struct NestedStruct {
        long m_long;
    };

    struct TopLevelStruct {
        String m_string;
        long m_array[2];
        NestedStruct m_complex;
        NestedStruct m_complex_array[2];
    };

The resulting type header row will look as follows:

.. list-table:: 
    :name: TableExampleTypeHeaderRow

    * - .m_string
      - .m_array[0]
      - .m_array[1]
      - .m_complex.a_long
      - .m_complex_array[0].a_long
      - .m_complex_array[1].a_long


.. note::

    Considerations about collection types: due to the column consistency
    required by the |CSV| format, the mapping of a collection type **requires
    to generate as many columns as elements can be present in the collection**,
    even if for a given data sample only a few of them are present (e.g., for
    ``Sequence`` type). If the recorded type has collections with big sizes,
    the generated file may hit the column limit of some |CSV| processor.


Unions
^^^^^^

The mapping of an ``Union`` type is similar to an ``Struct`` type except that
a discriminator column with name ``disc`` is placed before all the members.

Example
'''''''

Consider the following type described in IDL:

::

    union UnionType switch (long) {
        case 0:
        long case1;

        case 1:
        StructType case2;

        default:
        long case_default;
    };

    struct StructType {
        UnionType m_union;
    };

The resulting type header row will look as follows:

.. list-table::
    :name: TableUnionTypeHeaderRow

    * - .m_union.disc
      - .m_union.case1
      - .m_union.case2
      - .m_union.default


Data Values
^^^^^^^^^^^
For a given data sample, the value for each member is placed under the
corresponding column represented as a ``String``, which applies to all primitive
types. By default, enumerations are printed with their corresponding text label.

A value for a column may not available in per-sample basis. This may occur for
the following situations:

* A Sequence member that does not contain all the possible elements.
* An Union member, which can only set a member at a time.
* An optional member, which may or may not be set.

By default, the value of an empty member is represented as ``nil``.

Example
'''''''

Consider the following type described in IDL:

::

    struct StructType {
        sequence<long>, 2 m_sequence;
        @optional String m_optional;
    };

And two samples with the following values (represented in ``JSON``):

.. code:: JSON

    {
        "m_sequence": [1, 2],
        "m_optional": "hello"
    }

    {
        "m_sequence": [1],
        "m_optional":
    }

The resulting type header row and two data values row will look as follows:

.. list-table::
    :name: TableEmptyMembersExample

    * - .m_sequence[0]
      - .m_sequence[1]
      - .m_optional
    * - 1
      - 2
      - hello
    * - 1
      - nil
      - nil


Plug-in Configuration
---------------------

The plug-in can be configured through the use of the the properties shown in
table below:

.. list-table:: Plug-in Configuration Properties
    :name: TablePlug-inProperties
    :widths: 30 10 60
    :header-rows: 1

    * - Name |br|
        <base_name> = **rti.recording.utils_storage**
      - Value
      - Description
    * - **<base_name>.output_dir_path**
      - ``<string>``
      - Absolute or relative path to where generated file(s) are placed. |br|
        Default: **.** (working directory)
    * - **<base_name>.output_file_basename**
      - ``<string>``
      - Prefix for the name of the generated file(s). The file generated for
        each topic has the following name: |br|
        
            ``[OUTPUT_FILE_BASE_NAME]-[TOPIC_NAME]`` |br|

        If the option to merge the output file is enabled, then the final file
        name is equal to ``[OUTPUT_FILE_BASE_NAME]``. |br|
        Default: **csv_converted**
    * - **<base_name>.output_merge**
      - <boolean>
      - Specifies whether the generated files shall be consolidated into
        a single file. |br|
        Default: **true**
    * - **<base_name>.verbosity**
      - <integer> [0 - 5]
      - Sets the verbosity level of the plug-in. See ``rti::config::Verbosity``
        for the different levels available.
        Default: **1** (Exceptions)
    * - **<base_name>.csv.empty_member_value**
      - <string> 
      - Sets the value used for data members that are not present or empty.
        Default: **nil**
    * - **<base_name>.csv.enum_as_string**
      - <boolean>
      - Indicates whether values for enumeration members are printed as their
        corresponding label string or as an integer. |br|
        Default: **true**


Building
========

Below there are the instructions to build and use this plug-in. All the commands
and syntax used assume a Unix-based system. If you run this example in a different
architecture, please adapt the commands accordingly.

To build the example you will need first to run CMake to generate the build files.
From the directory containing the example sources:

::

    mkdir build
    cd build
    cmake -DCONNEXTDDS_DIR=<Connext directory> \
            -DCONNEXTDDS_ARCH=<ARCH> \
            -DCMAKE_BUILD_TYPE=Release \
            -DBUILD_SHARED_LIBS=ON ..
    cmake --build .


where:

- ``<Connext directory>`` shall be replaced with the path to the installation
  directory of *RTI Connext*.
- ``<ARCH>`` shall be replaced with the target architecture where you are
  running the example (e.g., x64Darwin15clang7.0).

.. note::

    For certain CMake generators you may need to provide the target platform
    through the ``-A`` command-line option. For example on Windows systems, you
    can provide the values ``Win32`` or ``x64``

.. note::

    Certain `CMake generators <https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html>`_
    generate multi-configuration files may need explicit selection of the
    configuration through ``--config``. For example for Visual Studio 2015, by
    default you can select ``Release`` or ``Debug``.

Upon success of the previous command it will create a shared library file in
the build directory.


Running
=======

To use this plug-in you will need to create a |RecS| configuration for the
*Converter* tool and select a custom storage. As part of this plug-in
distribution you can find an example file that selects and configures this
plug-in and converts all the *Topics* available in an input database in ``CDR``
format.

To run this example you will need to run *RTI Shapes Demo*  and |RecS| (for
*Recording* and *Converter* tools).

Setup
-----

If you run |RecS| from a different directory where the plug-in library is
located, you will need first to set up your environment to point to location
of the library. For example:

* Unix plaforms:

    ::

        export RTI_LD_LIBRARY_PATH=$PWD/Debug/x64

* Windows plaforms:

    ::

        set PATH=%PATH%;$PWD\Debug\x64

The steps to run the example in following section assumes you are running |RecS|
from the output build directory.

Execution
---------

#. Run one instance of |SD| on domain 0. This will be the publisher application.
   Publish blue squares and blue circles.


#. Run |RecS| to cause the recording data from the publisher application
   Run the following command from the example  build directory:

   ::

        <Connext directory>/bin/rtirecordingservice


   Run the service for a few seconds then press ``Ctrl+C`` to shutdown the service.
   On successful recording, you will find a directory with name ``cdr_recording``
   in the working directory containing the recorded shapes in a ``sqlite``
   database in ``CDR`` format.

#. Now run *Converter* to perform the conversion of the previously generated
   database into a single text file in |CSV| format:

   ::

        <Connext directory>/bin/rticonverter \
                -cfgFile ../ConverterToCSv.xml \
                -cfgName CdrToCsv


   Wait until conversion of the entire input database is perform and the
   application exits automatically. Upon successful conversion, you will
   find a file in the current directory with name ``ExampleCsv.csv`` that
   contains the converted content.
