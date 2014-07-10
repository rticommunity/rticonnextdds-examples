use Cwd;

$ARCH = "i86Linux2.6gcc4.4.5";
$NDDS_VERSION = "5.1.0";

#$TOP_DIRECTORY is the directory where you have executed the script
$TOP_DIRECTORY = cwd();

$NDDS_HOME = "";
# This variable is the NDDSHOME environment variable
# If NDDSHOME is defined, leave it as is, else it is defined by default
if (defined $ENV{'NDDSHOME'}) {
    $NDDS_HOME = $ENV{'NDDSHOME'};
}
else { 
    $NDDS_HOME = $ENV{'RTI_TOOLSDRIVE'} . "/local/preship/ndds/ndds." . 
                        $NDDS_VERSION;
}
#set NDDSHOME
$ENV{'NDDSHOME'} = $NDDS_HOME;

#include Java compiler (Javac) in the path
# If JAVAHOME is not defined we defined it by default
if (!defined $ENV{'JAVAHOME'}) {
    $ENV{'JAVAHOME'} = $ENV{'RTI_TOOLSDRIVE'} . "/local/applications/Java/" . 
        "PLATFORMSDK/linux/jdk1.7.0_04";
}

$ENV{'PATH'} = $ENV{'JAVAHOME'} . "/bin:" . $ENV{'PATH'};

#set LD_LIBRARY_PATH
#C/C++ architecture
$ENV{'LD_LIBRARY_PATH'} = $ENV{'NDDSHOME'} . "/lib/" . $ARCH . ":" . 
                            $ENV{'LD_LIBRARY_PATH'};
#Java Architecture
$ENV{'LD_LIBRARY_PATH'} = $ENV{'NDDSHOME'} . "/lib/" . $ARCH . "jdk:" . 
                            $ENV{'LD_LIBRARY_PATH'};                           


# This function runs the makefile generated with the rtiddsgen 
#   input parameter (they are used in the construction of the makefile name):
#       $architecture: the arechitecture which the example are going to be built
#       $language: this is the language which is going to be used to compile
#       $path: relative directory where the example is
#   output parameter:
#       none
sub call_makefile {
    my ($architecture, $language, $path) = @_;
    
    print_example_name ($path);
    
    my ($make_string) = "";
    if ($language eq "Java") {
        $make_string = "javac -classpath .:\"\$NDDSHOME\"/class/nddsjava.jar " . 
                        "*.java";
        print $make_string . "\n";
    } else {
        $make_string = "make -f makefile_Foo_" . $architecture;
    }

    #change to the directory where the example is (where the rtiddsgen has been
    # executed) to run the makefile
    chdir $path;
    
    system $make_string;
    if ( $? != 0 ) {
        exit(1);
    }
    
    # return to the top directory again
    chdir $TOP_DIRECTORY;
}

#This function prints the name of the example which is going to be compiled
sub print_example_name {
    my ($folder) = @_;
    print "\n*******************************************************" . 
          "****************\n";
    print "***** EXAMPLE: $folder\n";
    print "*********************************************************" . 
          "**************\n";
}

#dynamic data access union discriminator example
call_makefile ($ARCH, "C", 
        "./examples/dynamic_data_access_union_discriminator/c");

call_makefile ($ARCH, "C++", 
        "./examples/dynamic_data_access_union_discriminator/c++");

call_makefile ($ARCH, "Java", 
        "./examples/dynamic_data_access_union_discriminator/Java");
        
#dynamic data nested structs example       
call_makefile ($ARCH, "C", "./examples/dynamic_data_nested_structs/c");

call_makefile ($ARCH, "C++", "./examples/dynamic_data_nested_structs/c++");

call_makefile ($ARCH, "Java", "./examples/dynamic_data_nested_structs/java");

#dynamic data sequences example
call_makefile ($ARCH, "C", "./examples/dynamic_data_sequences/c");

call_makefile ($ARCH, "C++", "./examples/dynamic_data_sequences/c++");

call_makefile ($ARCH, "Java", "./examples/dynamic_data_sequences/java");
