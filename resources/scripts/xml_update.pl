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
use Data::Dumper;
use XML::Simple;
use File::Copy qw(move);

# Example of use:
#    perl xml_update.pl <working_directory> <option_flag> <ndds_version> <path_schema>

# The first command prompt argument is the directory to check
$FOLDER_TO_CHECK = $ARGV[0];

# The second argument is the option: 
#   0: checking if the xml has the attributes in the dds tag
#   1: adding the attributes in the dds tag if they are not written
#   2: updating the dds tag replacing the old attributes by the new ones
$OPTION_FLAG = $ARGV[1];

# The third command prompt argument is the ndds version you want to the xml
# files have
$NDDS_VERSION = $ARGV[2];

# The fourth command prompt argument is the path to the xsd file 
$XSD_PATH = $ARGV[3];

# Names of the schema files 
$RTI_DDS_QOS_PROFILES = "rti_dds_qos_profiles.xsd";
$RTI_DDS_PROFILES = "rti_dds_profiles.xsd";
$RTI_PERSISTENCE_SERVICE = "rti_persistence_service.xsd";
$RTI_ROUTING_SERVICE = "rti_routing_service.xsd";
$RTI_RECORD = "rti_record.xsd";
$RTI_REPLAY = "rti_replay.xsd";

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

# This function get the filename (name + extension) from a path including it
#   input parameter:
#       $path: the string to get the filename (including extension)
#   output parameter:
#       the filename
sub get_filename_from_path {
    my ($path) = @_;
    my ($filename) = "";
    ($filename = $path) =~ s/.*\///;
    return $filename;
}

# This function gets the name of the schema which the $xml_filename in pointing
#   input parameter:
#       $xml_filename: the filename to get the schema
#   output parameter:
#       $schema: the type of the schema where that XML file is pointing.
sub get_schema_from_file {
    my ($xml_filename) = @_;
    
    # we store our xml tree (all tag and attributes) in the $xml variable
    my $xs = XML::Simple->new( KeepRoot => 1, KeyAttr => 1, ForceArray => 1 );
    my $xml = $xs->XMLin($xml_filename);
   
    my $schema_location = $xml->{dds}[0]{'xsi:noNamespaceSchemaLocation'};
    $schema_location = unix_path($schema_location);
    my $schema_type = get_filename_from_path(unix_path($schema_location));
    
    return $schema_type;
}

# This function checks whether the xml has all the dds tag attributes
#   input parameter:
#       $xml_filename: the path to the xml file which are going to be checked
#   output parameter:
#       end_correct: 1 (True) or 0 (False), whether the xml has all the dds tag
#                    attirbutes or not
sub check_xml_dds_attributes {
    my ($xml_filename) = @_;

    my $xs = XML::Simple->new( KeepRoot => 1, KeyAttr => 1, ForceArray => 1 );
    my $xml = $xs->XMLin($xml_filename);
   
    # getting the xml parameters: version, xsi:noNamespaceSchemaLocation 
    # and xmlns:xsi
    my $version = $xml->{dds}[0]{'version'};
    my $xmlns = $xml->{dds}[0]{'xmlns:xsi'};
    my $schema_location = $xml->{dds}[0]{'xsi:noNamespaceSchemaLocation'};
    my $schema_type = get_filename_from_path(unix_path($schema_location));
    
    my $dds_tag_has_all_attributes = 0; 
     
    # checking if the XML file has all the 3 attributes
    if ($xmlns eq "" or $schema_location eq "") {
        $dds_tag_has_all_attributes = 0;
    } elsif ($version eq "" and !($schema_type eq $RTI_PERSISTENCE_SERVICE)) {
        $dds_tag_has_all_attributes = 0;
    } else {
        $dds_tag_has_all_attributes = 1;
    }
    
    return $dds_tag_has_all_attributes;
}

# This function adds the missed attributes to the dds tag in the xml file
#   input parameter:
#       $xml_filename: the path to the xml file which we are going to work
#       $new version: the version to add if they hasn't.
#       $new_schema_location: the path to the xsd schema.
#   output parameter:
#       $modified: 1 (True) or 0 (False), whether the xml has all the dds tag
#                  attirbutes or not
sub add_xml_dds_attributes {
    my ($xml_filename, $new_version, $new_schema_location) = @_;
    
    # we store our xml tree (all tag and attributes) in the $xml variable
    my $xs = XML::Simple->new( KeepRoot => 1, KeyAttr => 1, ForceArray => 1 );
    my $xml = $xs->XMLin($xml_filename);
   
    my $version = $xml->{dds}[0]{'version'};
    my $xmlns = $xml->{dds}[0]{'xmlns:xsi'};
    my $schema_location = $xml->{dds}[0]{'xsi:noNamespaceSchemaLocation'};
    my $schema_type = get_filename_from_path(unix_path($schema_location));
    my $new_schema_type = get_filename_from_path(
                                unix_path($new_schema_location));
    
    my $new_filename = $xml_filename . ".new";
    my $modified = 0;
    
    # we get the xml definition text, to add it in the new file with the
    # attributes in dds tag
    open(my $fh, '<:utf8', $xml_filename)
        or die "Could not open file '$filename' $!";
    
    # To copy all the file in a string
    local $/ = undef;
    my ($buffer) = <$fh>;
    close $fh;
    
    # We get the text before the <dds> tag, and after that tag
    $buffer =~ /([\s\S]*)<\s*dds[\s\S]*?>\n([\s\S]*)/;
    my ($text_before_dds_tag) = $1;
    my ($text_after_dds_tag) = $2;
       
    # if the attributes are empty, we write the new ones
    if ($version eq "") {
        if (!($schema_type eq $RTI_PERSISTENCE_SERVICE) and
            !($new_schema_type eq $RTI_PERSISTENCE_SERVICE)) {
            $xml->{dds}[0]{version} = $new_version;
            $modified = 1;
        }
    }
    
    if ($xmlns eq "") {
        $xml->{dds}[0]{'xmlns:xsi'} = 
            "http://www.w3.org/2001/XMLSchema-instance";
        $modified = 1;
    }
    
    if ($schema_location eq "") {
        $xml->{dds}[0]{'xsi:noNamespaceSchemaLocation'} = $new_schema_location;
        $modified = 1;
    }
    
    my ($dds_tag_version) = "";
    my ($dds_tag_schema_location) = "";
    my ($dds_tag_xmlns) = "";
    
    # we get the attributes from out $xml tree
    while(my ($param_key, $param_value) = each(%{$xml->{dds}[0]})) {
        if ($param_key eq "version") {
            $dds_tag_version = $param_value;
        }
        if ($param_key eq "xsi:noNamespaceSchemaLocation") {
            $dds_tag_schema_location = $param_value;
        }
        if ($param_key eq "xmlns:xsi") {
            $dds_tag_xmlns = $param_value;
        }
            
    }
    
    # we create the string with the dds tag we are going to replace in our file
    my ($dds_tag) = "<dds xmlns:xsi=\"$dds_tag_xmlns\"" .
          "\n     xsi:noNamespaceSchemaLocation=\"$dds_tag_schema_location\"";
    if (!($schema_type eq $RTI_PERSISTENCE_SERVICE) and
        !($new_schema_type eq $RTI_PERSISTENCE_SERVICE)) {
        $dds_tag .= "\n     version=\"$dds_tag_version\"";
    }
    $dds_tag .= ">\n";

    # if the xml has been modified, we moving the new file to the old file
    if ($modified) {
        open (my $fh, '>>:utf8', $new_filename);
        print $fh $text_before_dds_tag . $dds_tag . $text_after_dds_tag;
        close $fh or warn "$0: close $path: $!";
        move $new_filename, $xml_filename;
    }
    return $modified;
}

# This function replaces the attributes in the dds tag for the new ones
#   input parameter:
#       $xml_filename: the path to the xml file which we are going to work
#       $new version: the version to add if they hasn't.
#       $new_schema_location: the path to the xsd schema.
#   output parameter:
#       $modified: 1 (True) or 0 (False), whether the xml has all the dds tag
#                  attirbutes or not
sub replace_xml_dds_attributes {
    my ($xml_filename, $new_version, $new_schema_location) = @_;
    
    # we store our xml tree (all tag and attributes) in the $xml variable
    my $xs = XML::Simple->new( KeepRoot => 1, KeyAttr => 1, ForceArray => 1 );
    my $xml = $xs->XMLin($xml_filename);
   
    my $version = $xml->{dds}[0]{'version'};
    my $xmlns = $xml->{dds}[0]{'xmlns:xsi'};
    my $schema_location = $xml->{dds}[0]{'xsi:noNamespaceSchemaLocation'};
    my $schema_type = get_filename_from_path(unix_path($schema_location));
    
    my $new_filename = $xml_filename . ".new";
    
    # we get the xml definition text, to add it in the new file with the
    # attributes in dds tag
    open(my $fh, '<:utf8', $xml_filename)
        or die "Could not open file '$filename' $!";
    
    # To copy all the file in a string
    local $/ = undef;
    my ($buffer) = <$fh>;
    close $fh;
    
    # We get the text before the <dds> tag, and after that tag
    $buffer =~ /([\s\S]*)<\s*dds[\s\S]*?>\n([\s\S]*)/;
    my ($text_before_dds_tag) = $1;
    my ($text_after_dds_tag) = $2;
    
    # we ge the name of the schema we want to write in the file
    my $new_schema_type = get_filename_from_path(
                                    unix_path($new_schema_location));
    my ($modified) = 0;
    
    # if the xml and the new schema_type are the same one, then we can replace
    # the version and the path to this schema
    if ($schema_type eq $new_schema_type) {
        # we modify the old values for the new ones
        $xml->{dds}[0]{version} = $new_version;
        $xml->{dds}[0]{'xsi:noNamespaceSchemaLocation'} = $new_schema_location;
    } else {
        #if the schema is not the same one, then return 'not modified' value 
        return $modified;
    }  
    $xml->{dds}[0]{'xmlns:xsi'} = "http://www.w3.org/2001/XMLSchema-instance";
    
    my ($dds_tag_version) = "";
    my ($dds_tag_schema_location) = "";
    my ($dds_tag_xmlns) = "";
    
    # we get the attributes from out $xml tree
    while(my ($param_key, $param_value) = each(%{$xml->{dds}[0]})) {
        if ($param_key eq "version") {
            $dds_tag_version = $param_value;
        }
        if ($param_key eq "xsi:noNamespaceSchemaLocation") {
            $dds_tag_schema_location = $param_value;
        }
        if ($param_key eq "xmlns:xsi") {
            $dds_tag_xmlns = $param_value;
        }    
    }
    
    # we create the string with the dds tag we are going to replace in our file
    my ($dds_tag) = "<dds ";
    if (!$dds_tag_xmlns eq "") {
         $dds_tag .= "xmlns:xsi=\"$dds_tag_xmlns\"";
         $modified = 1;
    } 
    if (!$dds_tag_schema_location eq "") {
        $dds_tag .=
          "\n     xsi:noNamespaceSchemaLocation=\"$dds_tag_schema_location\"";
          $modified = 1;
    }
    if (!($dds_tag_version eq "") and 
        !($schema_type eq $RTI_PERSISTENCE_SERVICE)) {
            $dds_tag .= "\n     version=\"$dds_tag_version\"";
            $modified = 1;
    }
    
    $dds_tag .= ">\n";
    
    if ($modified) {
        # if the xml has been modified, we moving the new file to the old file
        open (my $fh, '>>:utf8', $new_filename);
        print $fh $text_before_dds_tag . $dds_tag . $text_after_dds_tag;
        close $fh or warn "$0: close $path: $!";
        move $new_filename, $xml_filename; 
    }    
    
    return $modified;
}

# This function reads recursively all the xml files in the $folder and it has
# three different options:
#       $OPTION_FLAG = 0: the script checks whether the <dds> tag has all 
#                         the 3 attributes
#       $OPTION_FLAG = 1: the script adds the attributes that the <dds> tag 
#                         did not have
#       $OPTION_FLAG = 2: the script replaces the attributes schema_location and 
#                         version when the <dds> tag's schema is pointing to the 
#                         same file than the introduced one by command line
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
        next if $register eq "."  or  $register eq ".." or 
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
            #if we only want to check
            if ($OPTION_FLAG == 0) {
                # if the xml has not their dds attributes, it exit with code 1
                if (!check_xml_dds_attributes ($file)) {
                    # if the xml fails
                    print "ERROR: The file has not the dds attributes: $file\n";
                    exit (1);
                } else {
                    # if the xml is properly formed
                    print "The file has the dds attributes: $file\n";
                }
            # if the <dds> tag has not a attribute, it is added
            } elsif ($OPTION_FLAG == 1) {
                if (add_xml_dds_attributes($file, $NDDS_VERSION, $XSD_PATH)) {
                    print "Added the attributes to the dds tag: $file\n";
                }
            # replacing all the <dds> tag attributes by the new ones   
            } elsif ($OPTION_FLAG == 2) {
                if (replace_xml_dds_attributes($file, $NDDS_VERSION, $XSD_PATH)){
                    print "Replaced the attributes to the dds tag: " .
					get_filename_from_path($file) . "\n";
                } else {
                    print "ERROR: The file " . get_filename_from_path($file) . 
					      " does not use the same schema than the introduced" .
						  " one: $XSD_PATH\n";
                }
            }
        } elsif (-d $file) {
            process_all_files($file);
        }
    }
}

process_all_files ($FOLDER_TO_CHECK);
