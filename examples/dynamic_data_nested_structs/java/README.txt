===============================================================
 Example Code -- Nested Struct in Dynamic Data
===============================================================

Building Java example
=====================

The example is contained in the DynamicDataNestedStruct.java file. Before 
compiling or running the example, make sure the environment variable NDDSHOME is
set to the directory where your version of RTI Connext is installed.

Before compiling in Java, make sure that the desired version of the javac 
compiler is in your PATH environment variable.

In Windows systems run:
    
    javac -classpath .;%NDDSHOME%\class\nddsjava.jar DynamicDataNestedStruct.java
    
On Unix systems (including Linux and MacOS X) run:

    javac -classpath .:$NDDSHOME/class/nddsjava.jar DynamicDataNestedStruct.java
    
There is a makefile, called makefilei86Linux2 to compile easier this example
on Unix systems.

$ make -f makefilei86Linux2.6jdk ARCH=<arch_name>

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

    java -cp .;%NDDSHOME%\class\nddsjava.jar DynamicDataNestedStruct

On UNIX systems:

    java -cp .:$NDDSHOME/class/nddsjava.jar DynamicDataNestedStruct
    
Running the program produces the following output:

================= BEGIN OUTPUT ==========================
 Connext Dynamic Data Nested Struct Example 
--------------------------------------------
 Data Types
------------------
struct InnerStruct {
   double x;
   double y;
}; //@Extensibility EXTENSIBLE_EXTENSIBILITY
struct OuterStruct {
   InnerStruct inner;
}; //@Extensibility EXTENSIBLE_EXTENSIBILITY


 get/set_complex_member API
------------------
Setting the initial values of struct with set_complex_member()

   inner: 
      x: 3.141590
      y: 2.718280

 + get_complex_member() called

 + inner struct value

   x: 3.141590
   y: 2.718280

 + setting new values to inner struct

 + current outter struct value 

   inner: 
      x: 3.141590
      y: 2.718280


 bind/unbind API
------------------

 + bind complex member called

   x: 3.141590
   y: 2.718280

 + setting new values to inner struct

   x: 1.000000
   y: 0.000010

 + current outter struct value 

   inner: 
      x: 1.000000
      y: 0.000010

================= END OUTPUT ==========================

