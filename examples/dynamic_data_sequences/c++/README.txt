======================================
 Example - Sequences in Dynamic Data
======================================

Building the example
====================

The example consists in only one file called "dynamic_data_sequences.cxx" that
shows how to access to complex sequence types.

A Linux makefile is provided, so it can be built with the following command line 

	$ make -f makefile_i86Linux2.6

Remember set $NDDHSOME and ARCH (e.g. i86Linux2.6gcc4.4.5) before running make. For instance
to build the example for i86Linux2.6gcc4.4.5

	$ make -f makefile_i86Linux2.6 ARCH=i86Linux2.6gcc4.4.5

To test the example using the bind API instead of using the get API, uncomment the following
line in the source file:

#define USE_BIND_API 

Running the example
===================

To run the example just run the generated binary executable, that is stored under the objs/<arch>/ with
the name dynamic_data_sequences. For instance, if the architecture is i86Linux2.6gcc4.4.5
just run this command: 

	$ objs/i86Linux2.6gcc4.4.5/dynamic_data_sequences
