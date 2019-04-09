  In the QoS profiles saved in an XML file, our root tag is <dds>. It needs some
attributes. Those attributes are included with rtiddsgen. However, there are old 
examples which have not it. Besides, in order to maintain the USER_QOS_PROFILES
in all the examples when the DDS version changes, we can use xml_update script 
to swap the DDS version and the version where it is pointing, because the XML 
validator, if the version is not the same one, will fail. A normal <dds> tag,
with the default attributes, is:

<dds xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
     xsi:noNamespaceSchemaLocation="http://community.rti.com/schema/6.0.0/rti_dds_qos_profiles.xsd"
     version="6.0.0">

  This script checks/modifies/replaces the attributes of the <dds> tag. It 
receives 4 arguments in the command line. The first one is the 
<working_directory>: the directory where the script are going to look for the 
XML files. The second parameter is the <option_flag>. The script receives the 
option flag in order to know which action we want to do (checking, modifying or 
replacing the attributes in the <dds> tag). Also, the script needs the 
<ndds_version> we are going to use for adding or replacing this attribute in the
<dds> tag. Finally, the script needs the path where the schema is, it could be a
global path in our computer, a relative path in our computer or an URL pointing 
to the schema. The <path_schema> is used to add or replace it in the <dds> tag 
in the XML file.

  You can call the script as follow:
    perl ./resources/scripts/xml_update.pl <working_directory> <option_flag> <ndds_version> <path_schema>
  
  Where:
  1. <working_directory> is the parent directory to check. 
  2. <option_flag> indicates which action the script is going to do. 
        - A value '0' indicates that the script is going to check whether the
          <dds> tag has all the 3 attributes which has been explained above.
        - A value '1' indicates that the script is going to add the attributes
          that the <dds> tag did not have. 
        - A value '2' indicates that the script is going to replace the 
          attributes <path_schema> and <ndds_version> when the <dds> tag's 
          schema is pointing to the same file than the introduced one by command
          line.
  3.- <ndds_version> is the version of RTI Connext DDS we are going to use and 
        write in the XML files (QoS profiles or services).
  4.- <path_schema> is the path where the XSD file is. 
          
  We need the <path_schema> and we do not use the one which is in the community 
portal because the script is going to check if the schema_name (the name of the
XSD file) is the same one we have written in the XML file. If it is the same 
one, the script could replace/update the VERSION and the schema_path attributes
in the XML file. Besides, we need to indicate which kind of schema we are going
to use (rti_dds_qos_profiles.xsd, rti_persistence_service.xsd ...) 

  Furthermore, if the schema we are using is rti_persistence_service.xsd, we 
will not copy/replace the dds version, because it does not need it.
