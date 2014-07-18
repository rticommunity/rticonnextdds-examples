  The script xml_validation.pl receives 2 arguments in command line. 
The first one is the parent directory to check, including its child directories.
The second one is the path where the XSD scheme is. 
  
  You can call this script:
    ./resources/scripts/xml_validation.pl <working_directory> <path_schema>
  
  Where:
  1. <working_directory> is the parent directory to check. 
  2. <path_schema> is the path where the XSD file is. 
  
  Normally, the most of the files use the rti_dds_qos_profiles.xsd. This script
checks all the .xml files which use that scheme. There are other files which do
not use this schema and the script will skip them. After that, those files will
be checked manually.