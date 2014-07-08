  This script is based in the IDL files. It takes the name of the file and then
call the correct rtiddsgen sentence, using the architecture name and the 
programming language (depends of the folder where it is that example). For 
instance, if the example is under "c" subdirectory, the language to compile is 
C. However, if the working subdirectory is "java", this script modify a little 
bit its architecture in order to be correct and the programming language is 
Java.
  The script compile_windows.pl receives 3 arguments. The first one is the 
parent directory where you are going to build, including its child directories.
The second one is the ndds version that you are going to use to compile the 
files inside the parent directory. And the last one, is the architecture which 
the example are going to build on. 

  So, the general expression is:
    perl ./resources/scripts/compile_windows.pl <working_directory> <ndds_version> <architecture>
  1. <working_directory> is the parent directory to check.
  2. <ndds_version> is the version which is used to compile.
  3. <architecture> is the architecture name whath you are going to compile.
  
  A example of use is:
   perl ./resources/scripts/compile_windows.pl ./examples 5.1.0 i86Linux2.6gcc4.4.5
  
  You have to ensure the script runs in the top repository directory.
