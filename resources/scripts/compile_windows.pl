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

################################################################################
########################## GLOBAL VARIABLES ####################################
################################################################################

# The first command prompt argument is the directory to check
$FOLDER_TO_CHECK = $ARGV[0];
# $NDDS_VERSION is the version introduced by command line
$NDDS_VERSION = $ARGV[1];
# This variable is the architecture name
$ARCH = $ARGV[2];

# $TOP_DIRECTORY is the directory where you have executed the script
$TOP_DIRECTORY = cwd();

# Check whether the version is greater or equal than 5.2.0, where the new
# directory structure is used
$IS_NEW_DIR_STRUCTURE = 0;
if ($NDDS_VERSION ge "5.2.0") {
    $IS_NEW_DIR_STRUCTURE = 1;
}

$NDDS_HOME = "";
# This variable is the NDDSHOME environment variable
# If NDDSHOME is defined, leave it as is, else it is defined by default
if (defined $ENV{'NDDSHOME'}) {
    $NDDS_HOME = $ENV{'NDDSHOME'};
}
else {
    if ($IS_NEW_DIR_STRUCTURE) {
        $NDDS_HOME = $ENV{'RTI_TOOLSDRIVE'} . "/local/preship/ndds/ndds." .
                            $NDDS_VERSION . "/unlicensed/rti_connext_dds-" .
                            $NDDS_VERSION;
    } else {

        $NDDS_HOME = $ENV{'RTI_TOOLSDRIVE'} . "/local/preship/ndds/ndds." .
                            $NDDS_VERSION;
    }
    print "CAUTION: NDDSHOME is not defined, by default we set to\n\t" .
        "$NDDS_HOME\n";
}

# check wheter NDDS_HOME directoy exists
if (!-d $NDDS_HOME) {
    print "ERROR: The default NDDSHOME directory doesn't exists: $NDDS_HOME";
    exit (1);
}

# set NDDSHOME
$ENV{'NDDSHOME'} = unix_path($NDDS_HOME);

# set the scripts folder to the PATH
if ($IS_NEW_DIR_STRUCTURE) {
    $ENV{'PATH'} = $ENV{'NDDSHOME'} . "/bin;" . $ENV{'PATH'};
} else {
    $ENV{'PATH'} = $ENV{'NDDSHOME'} . "/scripts;" . $ENV{'PATH'};
}

# If OS_ARCH is not defined, we take it from the input architecture
if (!defined $ENV{'OS_ARCH'}) {
    $ENV{'OS_ARCH'} = substr $ARCH, 0, 8;
}

# we get the number of bit of the architecture, this is: "i86Win32" or
# "x64Win64"
$ARCHITECTURE_NUMBER_OF_BITS = $ENV{'OS_ARCH'};

# set PATH
if ($IS_NEW_DIR_STRUCTURE) {
    # C/C++/C# architecture
    $ENV{'PATH'} = $ENV{'NDDSHOME'} . "/lib/" . $ARCH . ";" . $ENV{'PATH'};
    # Java Architecture
    $ENV{'PATH'} = $ENV{'NDDSHOME'} . "/lib/java;" . $ENV{'PATH'};
} else {
    # C/C++/C# architecture
    $ENV{'PATH'} = $ENV{'NDDSHOME'} . "/lib/" . $ARCH . ";" . $ENV{'PATH'};
    # Java Architecture
    $ENV{'PATH'} = $ENV{'NDDSHOME'} . "/lib/" . $ENV{'OS_ARCH'} . "jdk;" .
                $ENV{'PATH'};
}

# including Java compiler (Javac) in the path
# If JAVAHOME is not defined we defined it by default
if (!defined $ENV{'JAVAHOME'}) {
    $ENV{'JAVAHOME'} = $ENV{'RTI_TOOLSDRIVE'} . "/local/applications/Java/" .
        "PLATFORMSDK/win32/jdk1.7.0_04";
    print "CAUTION: JAVAHOME is not defined, by default we set to\n\t" .
        "$ENV{'JAVAHOME'}\n";
}

$ENV{'PATH'} = $ENV{'JAVAHOME'} . "/bin;" . $ENV{'PATH'};

# Global variable to save the language to compile, will be modified with every
# compile string we launch
$LANGUAGE = "";

# This is the dot net version
$DOT_NET_VERSION = "dotnet4.0";

################################################################################
######################### AUXILIAR FUNCTIONS ###################################
################################################################################

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

################################################################################
##################### BUILD AND COMPILE FUNCTIONS ##############################
################################################################################

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

    # if the compilation language is Java, we have to add jdk at the final of
    # the $ARCH
    if ($language eq "Java") {
        if ($IS_NEW_DIR_STRUCTURE) {
            $architecture = $ARCH;
        } else {
            $architecture = $ARCHITECTURE_NUMBER_OF_BITS . "jdk";
        }
    }
    # if the compilation language is C#, we have to add the dot net version
    # and disable the preprocessor
    elsif ($language eq "C#") {
        if ($IS_NEW_DIR_STRUCTURE) {
            $architecture = $ARCH;
        } else {
            $architecture = $ARCHITECTURE_NUMBER_OF_BITS . $DOT_NET_VERSION .
                        " -ppDisable ";
        }
    }
    my ($call_string) = "";

    # create the string to call rtiddsgen
    $call_string =  "rtiddsgen -language " . $language . " -example " .
                    $architecture . " " . $idl_filename;

    system $call_string;
    # if the system call has errors, the program exits with the error code 1
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

    # delete the idl file name from the path, and we have the folder where the
    # idl file is
    my ($execution_folder) = substr $path, 0, (-1 * length $idl_filename);

    my ($compile_string) = "";
    my ($visual_studio_year) = substr $ARCH, length($ARCH) -4;

    # we create the compile string for visual studio projects, which is the same
    # one for C and C++ languages
    if ($LANGUAGE eq "C" or $LANGUAGE eq "C++" or $LANGUAGE eq "C++03" or
        $LANGUAGE eq "C++11") {

        $compile_string = "msbuild " . $data_type . "-vs" .
                $visual_studio_year . ".sln";
    }
    # if the language is Java, all the files will be compiled with *.java
    elsif ($LANGUAGE eq "Java") {
        if ($IS_NEW_DIR_STRUCTURE) {
            $compile_string = "javac -classpath .;\"%NDDSHOME%\"\\lib\\java\\" .
                              "nddsjava.jar *.java";
        } else {
            $compile_string = "javac -classpath .;\"%NDDSHOME%\"\\class\\" .
                              "nddsjava.jar *.java";
        }
        print $compile_string . "\n";
    }
    # if the language is C# we create the compile string for visual studio C#
    # projects
    elsif ($LANGUAGE eq "C#") {
        # Firstly, compiling type
        $compile_string = "msbuild " . $data_type . "-csharp.sln";
    }

    # change to the directory where the example is (where the rtiddsgen has been
    # executed) to run the makefile
    chdir $execution_folder;

    # if the system call has errors, the program exits with the error code 1
    system $compile_string;
    if ( $? != 0 ) {
        # return to the top directory again
        chdir $TOP_DIRECTORY;
        exit(1);
    }

    # return to the top directory again
    chdir $TOP_DIRECTORY;
}


# This function reads recursively all the files in a folder and call:
#   - call_rtiddsgen: this function generates all the needed files to compile,
#                     using rtiddsgen
#   - call_makefile: this function calls the makefile to compile the example
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
            } elsif ($register eq "c++03") {
                $LANGUAGE = "C++03";
            } elsif ($register eq "c++11") {
                $LANGUAGE = "C++11";
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
