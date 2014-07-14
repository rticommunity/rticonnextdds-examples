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
# Example of use:
#  perl write_copyright.pl <working_directory> <option_flag> <extensions>...

use Cwd;
use File::Copy qw(move);

################################################################################
########################## GLOBAL VARIABLES ####################################
################################################################################

$COPYRIGHT_FILENAME_C_STYLE = "resources/copyright_c_style.txt";
$COPYRIGHT_FILENAME_XML_STYLE = "resources/copyright_xml_style.txt";

# The next function will load the number of character on the copyright text
$COPYRIGHT_LENGTH_C_STYLE = chars_per_file($COPYRIGHT_FILENAME_C_STYLE);
$COPYRIGHT_LENGTH_XML_STYLE = chars_per_file($COPYRIGHT_FILENAME_XML_STYLE);

# This variable is the number of bytes of the copyright.txt file. It is needed
# to seek in the file
$COPYRIGHT_SIZE_C_STYLE = -s $COPYRIGHT_FILENAME_C_STYLE;
$COPYRIGHT_SIZE_XML_STYLE = -s $COPYRIGHT_FILENAME_XML_STYLE;

# The next function will load the copyright text
$COPYRIGHT_TEXT_C_STYLE = read_text_w_length($COPYRIGHT_FILENAME_C_STYLE , 
                        $COPYRIGHT_LENGTH_C_STYLE);

$COPYRIGHT_TEXT_XML_STYLE = read_text_w_length($COPYRIGHT_FILENAME_XML_STYLE , 
                        $COPYRIGHT_LENGTH_XML_STYLE);

# The next variable will have the extensions of the files which are going to
# be examined
$EXTENSIONS = get_extensions();

# This variable is the file-name where the black list is
$BLACK_LIST_FILENAME = "resources/black_list.txt";

# The first command prompt argument is the directory to check
$FOLDER_TO_CHECK = $ARGV[0];
# The second comand prompt argument is a flag (0, 1 or 2)
#   Value '0': JUST CHECK IF THE FILES HAS COPYRIGHT OR NOT.
#   Value '1': COPY THE COPYRIGHT HEADER IN THE FILES WHICH HAVE NOT IT.
#   Value '2': DELETE THE COPYRIGHT HEADER IN THE FILES WHICH HAVE IT.
$OPTIONS_FLAG = $ARGV[1];

################################################################################
######################### AUXILIAR FUNCTIONS ###################################
################################################################################

# This function parse the typed extensions in command line
#   input parameter:
#       @ARGV: the arguments in command line
#   output parameter:
#       $buffer: a string with all the typed extension separated by '|'. This
#           character is used by perl like logical OR in its regular expressions
sub get_extensions {
    my ($buffer) = "";
    
    for($i = 2; $i < scalar(@ARGV); $i++) {
        $buffer .= $ARGV[$i] . "|";
    }
    chop($buffer);
    
    return $buffer;
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

# This function count the number of characters from one file.
#   input parameter:
#       $filename: the name of the file where the characters have been counted.
#   output parameter:
#       $chars: the number of character that the file has.
sub chars_per_file {
    my ($filename) = @_;
    open(my $fh, '<:utf8', $filename) 
        or die "Could not open file $filename: $!";
    my $chars = 0;
    
    while (<$fh>) {
        $chars += length($_);
    }
    
    close $fh;
    
    return $chars;
}

# This function read from a file $length characters and return that string
#   input parameter:
#       $filename: the name of the file where the string will be read
#       $length: the number of characters which the string are going to read
#   output parameter:
#       $buffer: an string with the $length firts character from $filename
sub read_text_w_length {
    my ($filename, $length) = @_;
    my ($buffer) = "";
    open(my $fh, '<:utf8', $filename)
        or die "Could not open file '$filename' $!";
 
    read($fh, $buffer, $length);
    
    close $fh;
    
    return $buffer;
}



################################################################################
######################### COPYRIGHT FUNCTIONS ##################################
################################################################################


# This function check if the $filename has written the copyright
#   input parameter:
#       $filename: the name of the file to check if it has copyright
#   output parameter:
#       $have_copyright: 1 (True) or 0 (False), if the $filename has the 
#                        copyright
sub check_if_copyright {
    my ($filename, $is_xml_file) = @_;
    my ($copyright) = "";
    open(my $fh, '<:utf8', $filename)
        or die "Could not open file '$filename' $!";
   
    if($is_xml_file) {
        read($fh, $copyright, $COPYRIGHT_LENGTH_XML_STYLE);
    } else {
        read($fh, $copyright, $COPYRIGHT_LENGTH_C_STYLE);
    }
    
    close $fh;
            
    my ($have_copyright) = 0;
    
    if ($is_xml_file) {
        if ($copyright eq $COPYRIGHT_TEXT_XML_STYLE) {
            $have_copyright = 1;
        }
    } else {
        if ($copyright eq $COPYRIGHT_TEXT_C_STYLE) {
            $have_copyright = 1;
        }
    }
    return $have_copyright;
}

# This function will copy the copyright text to a file named $filename
#   input parameter:
#       $filename: the name of the file to copy the copyright text
#   output parameter:
#       none
sub copy_copyright_in_file {
    my ($filename, $is_xml_file) = @_;
    #buffer will store the text file
    my ($buffer) = "";
    open(my $fh, '+<:utf8', $filename)
        or die "Could not open file '$filename' $!";
    local $/ = undef;
    
    $buffer = <$fh>;
    #set the file handle at the start of the file
    seek $fh, 0, 0;
    
    if ($is_xml_file) {
        print $fh $COPYRIGHT_TEXT_XML_STYLE.$buffer;
    } else {
        print $fh $COPYRIGHT_TEXT_C_STYLE.$buffer;
    }

    close $fh;
}

# This function will copy the copyright text to a file named $filename
#   input parameter:
#       $filename: the name of the file to copy the copyright text
#   output parameter:
#       none
sub delete_copyright_in_file {
    my ($filename, $is_xml_file) = @_;
    #buffer will store the text file
    my ($buffer) = "";
    my ($new_filename) = $filename.".new";
    
    open(my $fh, '+<:utf8', $filename)
        or die "Could not open file '$filename' $!";
    open (my $new_fh, '>>:utf8', $new_filename);
    
    #set the file handle at the end of the copyright header
    if ($is_xml_file) {
        seek $fh, $COPYRIGHT_SIZE_XML_STYLE, 0;
    } else {
        seek $fh, $COPYRIGHT_SIZE_C_STYLE, 0;
    }
    
    while (<$fh>) {
        my $line = $_;
        print $new_fh $line;
    }
    
    close $new_fh;
    close $fh;
    unlink $fh;
    move $new_filename, $filename;
}

# This function will check if a string is in a file
#   input parameter:
#       $filename: the name of the file which contains the strings
#       $folder_to_check: it is the string (folder_name) which we will check
#           if it is in the file
#   output parameter:
#       $black_list: 1 (True) or 0 (False), if the $filename contains the folder
sub is_in_the_black_list {
    my ($filename, $folder_to_check) = @_;
    my ($black_list) = 0; #false by default
    
    open (my $fh, '<:utf8', $filename)
        or die "Could not open file '$filename': $!";
    
    while ($line = <$fh>) {
        chomp($line);
        $line = unix_path($line);
        if ($line eq $folder_to_check) {
            $black_list = 1;
            last;
        }
    }
    return $black_list;    
}

# This function read recursively all the files in a folder and process them:
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
        next if $register eq "."  or  $register eq "..";
        
        my $file = "$folder/$register";
        $file = unix_path($file);
        
        my ($is_xml_file) = 0;
        
        # If a file has been found-> check if it has copyright
        if (-f $file) {
            # If the file has not one of the introduced extensions -> do nothing
            next if $file !~ /\.($EXTENSIONS)$/i;
            
            if (substr ($register, 0, -4) eq ".xml") {
                $is_xml_file = 1;
            }
            
            print "Text file Checking: $file\n";
            ##### ACTIONS IF THE FILE HAS THE COPYRIGHT OR NOT ######
            if ( check_if_copyright ($file, $is_xml_file) ) {
                print "File $file has the copyright written\n";  
                
                #delete copyright header
                if ($OPTIONS_FLAG == 2) {
                    delete_copyright_in_file($file, $is_xml_file);
                    print "File $file: copyright header deleted\n";
                }
            } else {
                #copy copyright header
                if ($OPTIONS_FLAG == 1) {
                    copy_copyright_in_file($file, is_xml_file);
                    print "File $file: copyright header copied\n";
                }
            }            

        } elsif (-d $file) {
            if (! is_in_the_black_list($BLACK_LIST_FILENAME, $file) ) {
                process_all_files($file);
            }
        }
    }
}

process_all_files($FOLDER_TO_CHECK);
