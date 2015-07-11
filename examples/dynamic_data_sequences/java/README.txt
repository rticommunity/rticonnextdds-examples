======================================
 Example - Sequences in Dynamic Data
======================================

Building Java Example
=====================
The example is contained in the DynamicDataSequences.java file. Before compiling
or running the example, make sure the environment variable NDDSHOME is set to 
the directory where your version of RTI Connext is installed.

Before compiling in Java, make sure that the desired version of the javac 
compiler is in your PATH environment variable.

In Windows systems run:
    
    javac -classpath .;%NDDSHOME%\lib\java\nddsjava.jar DynamicDataSequences.java
    
On Unix systems (including Linux and MacOS X) run:

    javac -classpath .:$NDDSHOME/lib/java/nddsjava.jar DynamicDataSequences.java

Running Java Example
====================

On Windows Systems:

    java -cp .;%NDDSHOME%\lib\java\nddsjava.jar DynamicDataSequences

On UNIX systems:

    java -cp .:$NDDSHOME/lib/java/nddsjava.jar DynamicDataSequences
