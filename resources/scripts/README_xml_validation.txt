  This script uses a environment variable called XMLLINT_PATH in order to  
point where the xmllint program is installed. If this environment variable is 
not defined, the script supposes the xmllint is in the path.
  The script xml_validation.pl receives 2 arguments in command line. 
The first one is the parent directory to check, including its child directories.
The second one is the ndds_version which the XSD file is using. 
  
  You can call this script:
    ./resources/scripts/xml_validation.pl <working_directory> <ndds_version>
  
  Where:
  1. <working_directory> is the parent directory to check. 
  2. <ndds_version> is the ndds_version which the XSD file is using. 
  
  Normally, the most of the files use the rti_dds_qos_profiles.xsd. This script
checks all the .xml files which use that scheme. There are other files which do
not use this schema and the script will skip them. After that, those files will
be checked manually using the global variables in the code to make easier adding
new examples with a schema different to rti_dds_qos_profiles.xsd.