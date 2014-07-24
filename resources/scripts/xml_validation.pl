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

# The second command prompt argument is the path to the xsd file 
$XSD_PATH = $ARGV[1];

# Path to the xmllint, if it is empty, the tool is in the path
if (!defined $ENV{'XMLLINT_PATH'}) {
    $ENV{'XMLLINT_PATH'} = "";
}

# This function change the '\' character by '/' like is used in UNIX
#   input parameter:
#       $path: the string to be converted
#   output parameter:
#       $path = $path using UNIX format
sub unix_path {
    my ($path) = @_;
    ($path = shift) =~ tr!\\!/!;
    return $path;
}

# This function calls a xml validator to check if a xml file is correct 
# following a xsd schema 
#   input parameter:
#       $xml_filename: the language which the example are going to be compiled
#       $xsd_path: the arechitecture which the example are going to be built
#   output parameter:
#       $end_correct: 1 (True) or 0 (False), if the file has error or not
sub validate_xml {
    my ($xml_filename, $xsd_path) = @_;
    my ($call_string) = "";
   
    $call_string =  $ENV{'XMLLINT_PATH'} ."xmllint --noout --schema " . $xsd_path . 
                        " " . $xml_filename; 
      
    system $call_string;
    
    if ( $? != 0 ) {
        exit(1);
    }
}


# This function reads recursively all the files in a folder and process them:
#   - if a file is found: check if its extension is supported
#           - if the file has not a supported extension: look for a new file
#           - if the file has a supported extension: check if it has copyright
#               - if the file has copyright: print a advise
#                    - if delete option is enabled: delete the copyright header
#               - else and enabled copy copyright option: copy the copyright 
#                       header in the file
#
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
        
        # if we find a idl file -> run rtiddsgen and then built it with the 
        # generated make
        if (-f $file) {
            next if $file !~ /\.xml$/i;
            print "\n*******************************************************" . 
                "****************\n";
            print "***** EXAMPLE: $folder\n";
            print "*********************************************************" . 
                "**************\n";
            
            # if validate_xml has any error, it exit with code 1
            validate_xml ($file, $XSD_PATH);
                
        } elsif (-d $file) {
            process_all_files($file);
        }
    }
}

process_all_files ($FOLDER_TO_CHECK);

# validate xml files in persistent_storage example
print "\n*******************************************************" . 
                "****************\n";
print "***** EXAMPLE: persistent_storage\n";
print "*********************************************************" . 
                "**************\n";
validate_xml ("./examples/persistent_storage/c/USER_QOS_PROFILES.xml", 
                        $XSD_PATH);

validate_xml ("./examples/persistent_storage/c/" . 
                "persistence_service_configuration.xml", 
        "http://community.rti.com/schema/5.1.0/rti_persistence_service.xsd");
                
validate_xml ("./examples/persistent_storage/c++/USER_QOS_PROFILES.xml", 
                        $XSD_PATH);

validate_xml ("./examples/persistent_storage/c++/" . 
                "persistence_service_configuration.xml",
        "http://community.rti.com/schema/5.1.0/rti_persistence_service.xsd");

validate_xml ("./examples/persistent_storage/cs/USER_QOS_PROFILES.xml", 
                        $XSD_PATH);

validate_xml ("./examples/persistent_storage/cs/" . 
                "persistence_service_configuration.xml",
        "http://community.rti.com/schema/5.1.0/rti_persistence_service.xsd");

validate_xml ("./examples/persistent_storage/java/USER_QOS_PROFILES.xml", 
                        $XSD_PATH);

validate_xml ("./examples/persistent_storage/java/" . 
                "persistence_service_configuration.xml",
        "http://community.rti.com/schema/5.1.0/rti_persistence_service.xsd");

# validate xml files in routing_service_file_adapter
print "\n*******************************************************" . 
                "****************\n";
print "***** EXAMPLE: routing_service_file_adapter\n";
print "*********************************************************" . 
                "**************\n";

validate_xml ("./examples/routing_service_file_adapter/c/file_bridge.xml", 
            "http://community.rti.com/schema/5.1.0/rti_routing_service.xsd");
                    
# validate xml files in writing_data_lua
print "\n*******************************************************" . 
                "****************\n";
print "***** EXAMPLE: writing_data_lua\n";
print "*********************************************************" . 
                "**************\n";

validate_xml ("./examples/writing_data_lua/prototyper_config.xml",
                "http://community.rti.com/schema/5.1.0/rti_dds_profiles.xsd");