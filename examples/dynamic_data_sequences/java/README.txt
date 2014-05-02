============================================================
 Example Code -- Access Union Discriminator In Dynamic Data
============================================================

Building Java Example
=====================
The example is contained in the UnionExample.java file. Before compiling or 
running the example, make sure the environment variable NDDSHOME is set to the 
directory where your version of RTI Connext is installed.

Before compiling in Java, make sure that the desired version of the javac 
compiler is in your PATH environment variable.

In Windows systems run:
    
    javac -classpath .;%NDDSHOME%\class\nddsjava.jar UnionExample.java
    
On Unix systems (including Linux and MacOS X) run:

    javac -classpath .:$NDDSHOME/class/nddsjava.jar UnionExample.java

Running Java Example
====================

Before running, make sure that the native Java libraries on which RTI Connext
depends are in your environment path (or library path). To add Java libraries 
to your environment...

On Windows systems run: 
set PATH=%NDDSHOME%\lib\i86Win32jdk;%PATH%

On Unix systems except MacOS X (assuming you are using Bash) run:
export LD_LIBRARY_PATH=$NDDSHOME/lib/<platform_name>jdk:$LD_LIBRARY_PATH

On MacOSX (assuming your are using Bash) run:
export DYLD_LIBRARY_PATH=$NDDSHOME/lib/<platform_name>jdk:$DYLD_LIBRARY_PATH

Run the following command from the example directory to execute
the application.

On Windows Systems:

    java -cp .;%NDDSHOME%\class\nddsjava.jar UnionExample

On UNIX systems:

    java -cp .:$NDDSHOME/class/nddsjava.jar UnionExample
