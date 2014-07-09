$NDDS_VERSION = "5.1.0";

#$TOP_DIRECTORY is the directory where you have executed the script
$TOP_DIRECTORY = $ENV{'PWD'};

# This variable is the NDDSHOME environment variable
$NDDS_HOME = $ENV{'RTI_TOOLSDRIVE'} . "/buildtools/windows/local/preship/" . 
             "ndds/ndds." . $ARGV[1];

#set NDDSHOME
$ENV{'NDDSHOME'} = $NDDS_HOME;
print "NDDSHOME: " . $ENV{'NDDSHOME'} . "\n";
#set the scripts folder to the PATH
$ENV{'PATH'} = $ENV{'NDDSHOME'} . "/scripts:" . $ENV{'PATH'};

#include Java compiler (Javac) in the path
$ENV{'PATH'}=$ENV{'RTI_TOOLSDRIVE'} . "/Buildtools/Windows/local/" . 
    "applications/Java/PLATFORMSDK/win32/jdk1.7.0_04/bin;" . $ENV{'PATH'};

#This solution name is the same one for all dynamic data C/C++ examples
$VS_SOLUTION_NAME_C = "Hello-i86Win32VS2010.sln";

#This solution name is the same one for all dynamic data C# examples
$VS_SOLUTION_NAME_CS = "Hello-i86Win32dotnet4.0.sln";

# This function runs the makefile generated with the rtiddsgen 
#   input parameter (they are used in the construction of the makefile name):
#       $language: this is the language which is going to be used to compile
#       $path: relative directory where the example is
#   output parameter:
#       none
sub call_compiler {
    my ($language, $path) = @_;
    
    print_example_name ($path);
    
    my ($compile_string) = "";
    
    if ($language eq "C" or $language eq "C++") {
        $compile_string = "msbuild " . $VS_SOLUTION_NAME_C;
    }
    elsif ($language eq "Java") {
        $compile_string = "javac -classpath .;\"%NDDSHOME%\"/class/nddsjava.jar"
                    . " *.java";
        print $compile_string . "\n";
    } elsif ($language eq "C#") {
        $compile_string = "msbuild " .$VS_SOLUTION_NAME_CS;
    }
    print "Compile string: <" . $compile_string . ">";
    
    #change to the directory where the example is (where the rtiddsgen has been
    # executed) to run the makefile
    chdir $path;
    
    system $compile_string;
    if ( $? == -1 ) {
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
call_compiler ("C", "./examples/dynamic_data_access_union_discriminator/c");

call_compiler ("C++", "./examples/dynamic_data_access_union_discriminator/c++");

call_compiler ("C#", "./examples/dynamic_data_access_union_discriminator/cs");

call_compiler ("Java", 
            "./examples/dynamic_data_access_union_discriminator/java");
        
#dynamic data nested structs example       
call_compiler ("C", "./examples/dynamic_data_nested_structs/c");

call_compiler ("C++", "./examples/dynamic_data_nested_structs/c++");

call_compiler ("C#", "./examples/dynamic_data_nested_structs/cs");

call_compiler ("Java", "./examples/dynamic_data_nested_structs/java");

#dynamic data sequences example
call_compiler ("C", "./examples/dynamic_data_sequences/c");

call_compiler ("C++", "./examples/dynamic_data_sequences/c++");

call_compiler ("C#", "./examples/dynamic_data_sequences/cs");

call_compiler ("Java", "./examples/dynamic_data_sequences/java");
