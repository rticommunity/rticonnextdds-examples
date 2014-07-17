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
#    perl compile_windows.pl <working_directory> <ndds_version> <architecture>

# The first command prompt argument is the directory to check
$FOLDER_TO_CHECK = $ARGV[0];
#$TOP_DIRECTORY is the directory where you have executed the script
$TOP_DIRECTORY = cwd();

# This variable is the NDDSHOME environment variable
# If NDDSHOME is defined, leave it as is, else it is defined by default
if (defined $ENV{'NDDSHOME'}) {
    $NDDS_HOME = $ENV{'NDDSHOME'};
}
else { 
    $NDDS_HOME = $ENV{'RTI_TOOLSDRIVE'} . "/local/preship/ndds/ndds." . 
                        $ARGV[1];
}

#$NDDS_HOME = $ENV{'RTI_TOOLSDRIVE'} . "/local/preship/ndds/ndds." . $ARGV[1];
#$NDDS_HOME = $ENV{'RTI_DRIVE'} . "/local/preship/ndds/ndds." . $ARGV[1];

# This variable is the architecture name 
$ARCH = $ARGV[2];

#set NDDSHOME
$ENV{'NDDSHOME'} = unix_path($NDDS_HOME);

#set the scripts folder to the PATH
$ENV{'PATH'} = $ENV{'NDDSHOME'} . "/scripts;" . $ENV{'PATH'};

# If OS_ARCH is not defined, we take it from the input architecture
if (!defined $ENV{'OS_ARCH'}) {
    $ENV{'OS_ARCH'} = substr $ARCH, 0, 8;
}

# we get the number of bit of the architecture, this is: "i86Win32" or 
# "x64Win64"
$ARCHITECTURE_NUMBER_OF_BITS = $ENV{'OS_ARCH'};

#set PATH
#C/C++/C# architecture
$ENV{'PATH'} = $ENV{'NDDSHOME'} . "/lib/" . $ARCH . ";" . $ENV{'PATH'};
#Java Architecture
$ENV{'PATH'} = $ENV{'NDDSHOME'} . "/lib/" . $ENV{'OS_ARCH'} . "jdk;" . 
                $ENV{'PATH'};                           
#include Java compiler (Javac) in the path
# If JAVAHOME is not defined we defined it by default
if (!defined $ENV{'JAVAHOME'}) {
    $ENV{'JAVAHOME'} = $ENV{'RTI_TOOLSDRIVE'} . "/local/applications/Java/" . 
        "PLATFORMSDK/win32/jdk1.7.0_04";
}

$ENV{'PATH'} = $ENV{'JAVAHOME'} . "/bin;" . $ENV{'PATH'};

# Global variable to save the language to compile
$LANGUAGE = "";

# This is the dot net version
$DOT_NET_VERSION = "dotnet4.0";


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

# This function calls rtiddsgen for the idl file using the corresponding 
# architecture and language 
#   input parameter:
#       $language: the language which the example are going to be compiled
#       $architecture: the arechitecture which the example are going to be built
#       $idl_filename: the filename of the .idl file which contains the types
#   output parameter:
#       none
sub call_rtiddsgen {
    my ($language, $architecture, $idl_filename) = @_;

    #if the compilation language is Java, add jdk at the final of the $ARCH
    if ($LANGUAGE eq "Java") {
        $architecture = $ARCHITECTURE_NUMBER_OF_BITS . "jdk";
    } elsif ($LANGUAGE eq "C#") {
        $architecture = $ARCHITECTURE_NUMBER_OF_BITS . $DOT_NET_VERSION . 
                        " -ppDisable ";
    }
    my ($call_string) = "";
    
    $call_string =  "rtiddsgen -language " . $language . " -example " .
                    $architecture . " " . $idl_filename; 
      
    #print "call_string: <" . $call_string . ">\n";
    system $call_string;
    if ( $? != 0 ) {
        exit(1);
    }
}

# This function runs the makefile generated with the rtiddsgen 
#   input parameter (they are used in the construction of the makefile name):
#       $architecture: the arechitecture which the example are going to be built
#       $idl_filename: the filename of the .idl file which contains the types
#       $path: complete relative path for the idl file
#   output parameter:
#       none
sub call_compiler {
    my ($architecture, $idl_filename, $path) = @_;
    # $type has the name of the idle without the extension .idl
    my ($data_type) = substr $idl_filename, 0, -4;

    #if the compilation language is Java, add jdk at the final of the $ARCH
    if ($LANGUAGE eq "Java") {
        my ($architecture) = $ARCHITECTURE_NUMBER_OF_BITS . "jdk";
    }
    
    # delete the idl file name from the path, and we have the folder where the 
    # idl file is
    my ($execution_folder) = substr $path, 0, (-1 * length $idl_filename);
       
    my ($compile_string) = "";
    my ($visual_studio_year) = substr $ARCH, length($ARCH) -4;
    
    if ($LANGUAGE eq "C" or $LANGUAGE eq "C++") {
        $compile_string = "msbuild " . $data_type . "-vs" . 
                $visual_studio_year . ".sln";
    } elsif ($LANGUAGE eq "Java") {
        $compile_string = "javac -classpath .;\"%NDDSHOME%\"\\class\\" . 
                          "nddsjava.jar *.java";
        print $compile_string . "\n";
    } elsif ($LANGUAGE eq "C#") {
        # Firstly, compiling type 
        $compile_string = "msbuild " . $data_type . "-csharp.sln";
    }

    #print $compile_string;
    
    # change to the directory where the example is (where the rtiddsgen has been
    # executed) to run the makefile
    chdir $execution_folder;
          
    system $compile_string;
    if ( $? != 0 ) {
        exit(1);
    }
    # return to the top directory again
    chdir $TOP_DIRECTORY;
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
        next if $register eq "."  or  $register eq "..";
                
        my $file = "$folder/$register";
        $file = unix_path($file);
        
        # if we find a idl file -> run rtiddsgen and then built it with the 
        # generated make
        if (-f $file) {
            next if $file !~ /\.idl$/i;
            print "\n*******************************************************" . 
                "****************\n";
            print "***** EXAMPLE: $folder\n";
            print "*********************************************************" . 
                "**************\n";
            
            call_rtiddsgen ($LANGUAGE, $ARCH, $file);
            call_compiler ($ARCH, $register, $file);
            
        } elsif (-d $file) {
            if ($register eq "c") {
                $LANGUAGE = "C";
            } elsif ($register eq "c++") {
                $LANGUAGE = "C++";
            } elsif ($register eq "cs") {
                $LANGUAGE = "C#";
            } elsif ($register eq "java") {
                $LANGUAGE = "Java";
            }
            
            process_all_files($file);
        }
    }
}

process_all_files ($FOLDER_TO_CHECK);
