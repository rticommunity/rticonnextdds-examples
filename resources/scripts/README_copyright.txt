  The script copyright.pl receives a variable number of arguments. The
first one is the parent directory to check, including its child directories.
The second one is a flag to copy copyright in the corresponding files or just 
check if that files have it or not. The other arguments are the extensions of 
the files that you want to examine (you may write many extensions as you want).

  You have to ensure the script runs in the top repository directory.

  You need a file copyright_c_style.txt and copyright_xml_style.txt in the 
directory ./resources/. They contain the copyright text to be checked, copied
or deleted.

  If the script detects that the file with is working with is an XML file: 
it automatically checks/copies/deletes the copyright_xml_style after the XML
definition, so the XML files will look like:
        <? XML_DEFINITION ?>
        <!-- 
            COPYRIGHT HEADER
        -->
        <START_THE_XML_FILE>
            ...
        </END_THE_XML_FILE>
    
  You can call the script:
    ./resources/scripts/copyright.pl <working_directory> <option_flag> <extensions>...
  
  Where:
  1. <working_directory> is the parent directory to check. 
  2. <option_flag> If the copyright will be checked, copied or deleted. 
        - A value '0' indicates that the copyright only will be checked if the 
          files contain it or not. If you use this option and the file does
          not have copyright, the script will fail.
        - A value '1' indicates that the copyright will be copied in the files
          which do not have it. 
        - A value '2' indicates that the copyright will be deleted in the files
          which already have it.
  3. <extensions>... is a no limited list with all the extensions of the files 
     which are going to be checked.
  
  In order to know if some files has copyright (just checking for it), we can 
call the script as follow:
  
    ./resources/scripts/copyright.pl ./examples 0 c cxx cs xml idl h
  
  The script will check if the files with extensions .c .cxx .cs .xml .idl .h 
has copyright in the directory (and subdirectories): ./examples.

  If you want to copy the copyright in the files which do not have it:
     ./resources/scripts/copyright.pl ./examples 1 cxx cs xml idl h

  Now, the script are going to check the files with extensions .c .cxx .cs .xml 
.idl .h in the directory ./examples. If those files do not have the copyright
header, the script copies it, else does nothing. The XML files will be checked 
like it has been explained above.

  Maybe we want to skip some directories because they are external examples and
we do not want to copy the copyright header. The script needs a file called 
"external_examples.txt" in the next path:
      ./resources/external_examples.txt
  The directories which was written in this file will be skipped by the 
copyright.pl script. This file should contain the directories without finishing 
in "/" or "\". For example the next line in that file will skip the batching
example in all the languages:
      ./example/batching
