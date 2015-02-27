===========================================
 Example Code -- Routing Service File Adapter
===========================================

Building C Example
==================
Before compiling or running the example, make sure the environment variable 
NDDSHOME is set to the directory where your version of RTI Connext DDS is 
installed.

The example includes an example Makefile for the x64Linux2.6gcc4.4.5
architecture. You can modify the makefile to match the requirements of your 
specific platform. You will find the flags to use for each specific platform RTI
Routing Service supports in your installation under the 
"$ROUTINGHOME/adapters/file/make" directory. If you are using Visual Studio, you
will also find example Visual Studio solutions under 
"$ROUTINGHOME/adapters/file/windows". You can modify them to point to this 
example.

The example makefile copies the shared libraries that contain the adapter to 
$(ROUTINGHOME)/bin/$(ARCH). That means you will probably not need to 
modify your path to load the libraries. However, if you find problems loading the
libraries, you will need to add that directory to your Path, if you are using
Windows, or your LD_LIBRARY_PATH if you are using most of Unix-like systems.

Running C Example
=================

Before running the example, take a look at file_bridge.xml. It defines the different
settings to load and configure the adapter. You will need to specify on it the path 
to the input directory that will be scanned to check for files, as well as the path 
to the output directory where all the discovered files will be copied over. 

Once you have set the right paths in the XML configuration file, just run RTI
Routing Service like this:

./scripts/rtiroutingservice -cfgFile /path/to/file_bridge.xml -cfgName file_to_file

