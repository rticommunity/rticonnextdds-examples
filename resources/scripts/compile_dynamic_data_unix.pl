$ARCH = "i86Linux2.6gcc4.4.5";
$NDDS_VERSION = "5.1.0";

#$TOP_DIRECTORY is the directory where you have executed the script
$TOP_DIRECTORY = $ENV{'PWD'};

# This variable is the NDDSHOME environment variable
$NDDS_HOME = "/local/preship/ndds/ndds." . $NDDS_VERSION;

#set NDDSHOME
$ENV{'NDDSHOME'} = $NDDS_HOME;

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
        $make_string = "javac -classpath .:\"$NDDSHOME\"/class/nddsjava.jar *.java";
        print $compile_string . "\n";
    } else {
        $make_string = "make -f " . "makefile_Foo_" . $architecture;
    }

    #change to the directory where the example is (where the rtiddsgen has been
    # executed) to run the makefile
    chdir $path;
    
    system $make_string;
    
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

