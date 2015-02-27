===========================================
 Example Code -- get_publishers
===========================================

Building C# Example
===================
Before compiling or running the example, make sure the environment variable 
NDDSHOME is set to the directory where your version of RTI Connext is installed.

Run rtiddsgen with the -example option and the target architecture of your 
choice (e.g., i86Win32VS2010). The RTI Connext Core 
Libraries and Utilities Getting Started Guide describes this process in detail. 
Follow the same procedure to generate the code and build the examples. Do not 
use the -replace option.

On Windows systems (assuming you want to generate an example for 
i86Win32VS2010) run:

rtiddsgen -language C# -example i86Win32VS2010 -ppDisable Foo.idl

Note: If you are using Visual Studio Express add the -express option to the 
command, i.e.,

rtiddsgen -language C# -example i86Win32VS2010 -express -ppDisable Foo.idl

File C:\local\Foo\cs\Foo_publisher.cs already exists and will
not be replaced with updated content. If you would like to get a new file with 
the new content, either remove this file or supply -replace option.

This is normal and is only informing you that the subscriber/publisher code has
not been replaced, which is fine since all the source files for the example are
already provided.

Rtiddsgen generates two solutions for Visual Studio C++ and C#, that you will 
use to build the types and the C# example, respectively. First open  
Foo_type-dotnet4.0.sln and build the solution. Once you've done that, open
Foo_example-csharp.sln and build the C# example.

Running C# Example
==================
Run the following command from the example directory:

On Windows systems run:

       bin\<Debug|Release>\Foo_publisher