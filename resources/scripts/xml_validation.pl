################################################################################
# (c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software.  Licensee has the right to distribute object form only
# for use with RTI products.  The Software is provided "as is", with no warranty
# of any type, including any warranty for fitness for any purpose. RTI is under
# no obligation to maintain or support the Software.  RTI shall not be liable 
# for any incidental or consequential damages arising out of the use or 
# inability to use the software.
################################################################################

#!C:/Perl64/bin/perl.exe -w
use Cwd;

# Example of use:
#    perl xml_validation.pl <working_directory> <path_schema>

# The first command prompt argument is the directory to check
$FOLDER_TO_CHECK = $ARGV[0];

# The second command prompt argument is the version we are going to validate
# the xml files
$NDDS_VERSION = $ARGV[1];

# XSD_PATH is the path where the schemas are
$XSD_PATH = "http://community.rti.com/schema/" . $NDDS_VERSION . "/";

# Names of the schema files 
$RTI_DDS_QOS_PROFILES = "rti_dds_qos_profiles.xsd";
$RTI_DDS_PROFILES = "rti_dds_profiles.xsd";
$RTI_PERSISTENCE_SERVICE = "rti_persistence_service.xsd";
$RTI_ROUTING_SERVICE = "rti_routing_service.xsd";
$RTI_RECORD = "rti_record.xsd";
$RTI_REPLAY = "rti_replay.xsd";

# These variables are pointing to an specific schema in the community portal
$PATH_RTI_DDS_QOS_PROFILES = $XSD_PATH . $RTI_DDS_QOS_PROFILES;
$PATH_RTI_DDS_PROFILES = $XSD_PATH . $RTI_DDS_PROFILES;
$PATH_RTI_PERSISTENCE_SERVICE = $XSD_PATH . $RTI_PERSISTENCE_SERVICE;
$PATH_RTI_ROUTING_SERVICE = $XSD_PATH . $RTI_ROUTING_SERVICE;
$PATH_RTI_RECORD = $XSD_PATH . $RTI_RECORD;
$PATH_RTI_REPLAY = $XSD_PATH . $RTI_REPLAY;

# Path to the xmllint, if it is empty, the tool is in the global path
if (!defined $ENV{'XMLLINT_PATH'}) {
    $ENV{'XMLLINT_PATH'} = "";
}

# This function changes the '\' character by '/' like is used in UNIX
#   input parameter:
#       $path: the string to be converted
#   output parameter:
#       $path = $path using UNIX format
sub unix_path {
    my ($path) = @_;
    ($path = shift) =~ tr!\\!/!;
    return $path;
}
 
# This function calls an XML validator to check if a XML file is correct 
# following a xsd schema 
#   input parameter:
#       $xml_filename: the name of the file we want to validate
#       $xsd_path: the path to the schema (.xsd) we want our xml follows
#   output parameter:
#       $end_correct: 1 (True) or 0 (False), if the file has error or not
sub validate_xml {
    my ($xml_filename, $xsd_path) = @_;
    my ($call_string) = "";
   
    $call_string =  $ENV{'XMLLINT_PATH'} . "xmllint --noout --schema " . 
                        $xsd_path . " " . $xml_filename; 
    
    system $call_string;
    
    if ( $? != 0 ) {
        exit(1);
    }
}


# This function reads recursively all the files and validate all the .xml files.
# It will exits with error if the script cannot validate any file.
#   input parameter:
#       $folder: the name of the folder to read
#   output parameter:
#       none
sub process_all_files {
    my ($folder)  = @_;
 
    opendir DIR, $folder or die "ERROR trying to open $folder $!\n";
    my @files = readdir DIR;
    
    close DIR;

    foreach $register (@files) {
        # There are some examples which will be skipped because they use a
        # different xsd schema. The xmlvalidator will be called manually for
        # them with the corresponding xsd schema they need.
        next if $register eq "."  or  $register eq ".." or 
                $register eq "persistent_storage" or 
                $register eq "routing_service_file_adapter" or
                $register eq "writing_data_lua";
                
        my $file = "$folder/$register";
        $file = unix_path($file);
        
        # if we find a xml file we call the validate_xml function
        if (-f $file) {
            next if $file !~ /\.xml$/i;
            print "\n*******************************************************" . 
                "****************\n";
            print "***** EXAMPLE: $folder\n";
            print "*********************************************************" . 
                "**************\n";
            
            # if validate_xml has any error, it exit with code 1
            validate_xml ($file, $PATH_RTI_DDS_QOS_PROFILES);
                
        } elsif (-d $file) {
            process_all_files($file);
        }
    }
}

process_all_files ($FOLDER_TO_CHECK);

# validating the XML files in persistent_storage example
print "\n*******************************************************" . 
                "****************\n";
print "***** EXAMPLE: persistent_storage\n";
print "*********************************************************" . 
                "**************\n";
validate_xml ("./examples/persistent_storage/c/USER_QOS_PROFILES.xml", 
                        $PATH_RTI_DDS_QOS_PROFILES);

validate_xml ("./examples/persistent_storage/c/" . 
                "persistence_service_configuration.xml", 
                 $PATH_RTI_PERSISTENCE_SERVICE);
                
validate_xml ("./examples/persistent_storage/c++/USER_QOS_PROFILES.xml", 
                        $PATH_RTI_DDS_QOS_PROFILES);

validate_xml ("./examples/persistent_storage/c++/" . 
                "persistence_service_configuration.xml",
                $PATH_RTI_PERSISTENCE_SERVICE);

validate_xml ("./examples/persistent_storage/cs/USER_QOS_PROFILES.xml", 
                      $PATH_RTI_DDS_QOS_PROFILES);

validate_xml ("./examples/persistent_storage/cs/" . 
                "persistence_service_configuration.xml",
                $PATH_RTI_PERSISTENCE_SERVICE);

validate_xml ("./examples/persistent_storage/java/USER_QOS_PROFILES.xml", 
                       $PATH_RTI_DDS_QOS_PROFILES);

validate_xml ("./examples/persistent_storage/java/" . 
                "persistence_service_configuration.xml",
                $PATH_RTI_PERSISTENCE_SERVICE);

# validating XML files in routing_service_file_adapter example
print "\n*******************************************************" . 
                "****************\n";
print "***** EXAMPLE: routing_service_file_adapter\n";
print "*********************************************************" . 
                "**************\n";

validate_xml ("./examples/routing_service_file_adapter/c/file_bridge.xml", 
                $PATH_RTI_ROUTING_SERVICE);

# This example is commented because the rti_dds_profiles.xsd fails with older
# versions of xmllint                    
# validating XML files in writing_data_lua example
# print "\n*******************************************************" . 
#                "****************\n";
# print "***** EXAMPLE: writing_data_lua\n";
# print "*********************************************************" . 
#                "**************\n";

#validate_xml ("./examples/writing_data_lua/prototyper_config.xml",
#               $PATH_RTI_DDS_PROFILES);