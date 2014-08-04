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

# Example of use:
#    perl bundle_examples.pl <working_directory> <repository_name>
use Cwd;

################################################################################
########################## GLOBAL VARIABLES ####################################
################################################################################
 
# The first command prompt argument is the directory to check
$FOLDER_TO_CHECK = $ARGV[0];
# $TOP_DIRECTORY is the directory where you have executed the script
$TOP_DIRECTORY = cwd();

# This variable is the repository name got from the commmand line arguments
$REPOSITORY_NAME = $ARGV[1];

# This variable is the path to the repository folder 
$REPOSITORY_PATH = $TOP_DIRECTORY . "/" . $REPOSITORY_NAME;

$EXAMPLES_PATH = $TOP_DIRECTORY . $FOLDER_TO_CHECK;


################################################################################
######################### BUNDLING FUNCTIONS ###################################
################################################################################

# This function bundles the top directory with all the examples
#   input parameter:
#       $path: the path to the top directory
#   output parameter:
#       none, if there is any error, exit with error code 
sub bundle_top_directory {
    my ($path) = @_;
    my $examples_directory = $EXAMPLES_PATH;
    my $error = 0;
    
    # if a system call does not return 0, there is an error
    $error += system "mkdir -p $path/bundles/$REPOSITORY_NAME/$REPOSITORY_NAME";
    chdir "$path/bundles/$REPOSITORY_NAME";
    $error += system "cp -r $examples_directory/* ./$REPOSITORY_NAME";
    $error += system "zip -r " . $REPOSITORY_NAME . ".zip $REPOSITORY_NAME";
    $error += system "tar cvzf " . $REPOSITORY_NAME . 
                                                    ".tar.gz $REPOSITORY_NAME";
    $error += system "rm -rf $path/bundles/$REPOSITORY_NAME/$REPOSITORY_NAME";
        
    # return to the top directory again
    chdir $TOP_DIRECTORY;
    
    #if error is not 0, there was error in the execution
    if ($error) {
        print "Error in bundle_top_directory function\n";
        exit ($error);
    }
}

# This function bundles all the examples in a their corresponding folder
#   input parameter:
#       $path: the path to the top directory
#   output parameter:
#       none, if there is any error, exit with error code 
sub bundle_each_example {
    my ($path)  = @_;

    my $exampes_path = $EXAMPLES_PATH;
 
    opendir DIR, $exampes_path or die "ERROR trying to open $exampes_path $!\n";
    my @files = readdir DIR;
    
    close DIR;

    # we copy all the files to the repository_name folder
    system "cp -r * ./$REPOSITORY_NAME";
    # we delete the repository_name folder inside of itself
    system "rm -rf ./$REPOSITORY_NAME/$REPOSITORY_NAME";
    system "rm -rf ./$REPOSITORY_NAME/bundles/*";
    
    chdir $exampes_path;
    system "cp -r  * $REPOSITORY_PATH/examples";
    
    foreach $register (@files) {
        next if $register eq "."  or  $register eq ".."; 
        my $error = 0;
        # if a system call does not return 0, there is an error
        $error += system "zip -r " . $register . ".zip $register";
        $error += system "tar cvzf " . $register . ".tar.gz $register";
        $error += system "mkdir -p $REPOSITORY_PATH/bundles/$register";
        $error += system "mv " . $register . 
                                ".tar.gz $REPOSITORY_PATH/bundles/$register";
        $error += system "mv " . $register . 
                                ".zip $REPOSITORY_PATH/bundles/$register";
        
        # if error is not 0, there was error in the execution
        if ($error) {
            print "Error bundling $register example\n";
            exit ($error);
        } 
    }
        
    # return to the top directory again
    chdir $TOP_DIRECTORY;
}

################################################################################
############################# MAIN SCRIPT ######################################
################################################################################

# if the $REPOSITORY_NAME folder does not exist, the script creates it
if (!(-d $REPOSITORY_NAME)) {
    system "mkdir " . $REPOSITORY_NAME;
}

# if the bundles folder does not exist, the script creates it
if (!(-d "./bundles")) {
    system "mkdir bundles";
}

bundle_top_directory($TOP_DIRECTORY);
bundle_each_example ($TOP_DIRECTORY);

