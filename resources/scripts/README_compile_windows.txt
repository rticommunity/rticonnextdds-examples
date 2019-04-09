  The script compile_dynamic_data_windows.pl has the same conditions than  
compile_windows.pl and follow the same README.txt file.

  This script is based in the IDL files. It takes the name of the file and then
calls the correct rtiddsgen sentence, using the architecture name and the 
programming language (depends of the folder where that example is). For 
instance, if the example is under "c" subdirectory, the language to compile is 
C. However, if the folder is "java", this script creates the corresponding 
rtiddsgen & compilation sentences to build in Java.
  The script will set the next environment variables if they are not defined
(Setting your own environment variables is recommended):
    NDDSHOME: will be set to 
        "%RTI_TOOLSDRIVE%/local/preship/ndds/ndds." + version introduced by
            command line.
    JAVAHOME: will be set to 
        "%RTI_TOOLSDRIVE%/local/applications/Java/PLATFORMSDK/win32/jdk1.7.0_04"
        
  The script compile_windows.pl receives 3 arguments. The first one is the 
parent directory where you are going to build the repository, including its 
child directories. The second one is the ndds version that you are going to use 
to compile the source files. And the last one, is the architecture which the 
examples are going to build on. 

  So, the general expression is:
    perl ./resources/scripts/compile_windows.pl <working_directory> <ndds_version> <architecture>
  1. <working_directory> is the parent directory to check.
  2. <ndds_version> is the version which is used to compile.
  3. <architecture> is the architecture name which you are going to compile.
  
  A example of use is:
   perl ./resources/scripts/compile_windows.pl ./examples 6.0.0 i86Linux2.6gcc4.4.5
  
  You have to ensure the script runs in the top repository directory.
  
