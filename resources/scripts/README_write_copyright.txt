  The script write_copyright.pl receives a variable number of arguments. The
first one is the parent directory to check, including its child directories.
The second one is a flag to copy copyright in the corresponding files or just 
check if that files have it or not. The other arguments are the extensions of 
the files that you want to examine (you may write many extensions as you want).

  You have to ensure the script runs in the top repository directory.

  You need a file copyright.txt in the directory ./resources/copyright.txt which
contains the copyright text to be checked or copied.
  
  You can call the script:
    ./resources/scripts/write_copyright.pl <working_directory> <option_flag> <extensions>...
  
  Where:
  1. <working_directory> is the parent directory to check. 
  2. <option_flag> If the copyright will be checked, copied or deleted. 
        - A value '0' indicates that the copyright only will be checked if the 
          files contain it or not.
        - A value '1' indicates that the copyright will be copied in the files
          which do not have it. 
        - A value '2' indicates that the copyright will be deleted in the files
          which already have it.
  3. <extensions>... is a no limited list with all the extensions of the files 
     which are going to be checked.
  
  An execution example to check if some files has copyright is:
  
    ./resources/scripts/write_copyright.pl ./examples 0 txt c cxx
  
  The script will check if the files with extensions .txt .c .cxx has copyright
in the directory ./examples.

  If you want to copy the copyright in the files which do not have it:
     ./resources/scripts/write_copyright.pl ./examples 1 txt c cxx

  Now, the script are going to check the files with extensions .txt .c .cxx in 
has the directory ./examples. If that files do not have the copyright header,
copy it, else do nothing.

  A 'black list' can be used. The directories (or subdirectories) included in 
this list will be skipped. This 'black list' have to be saved in 
./resources/black_list.txt

  The black_list.txt should contain the directories without finishing in "/" 
or "\". For example ./example/batching
