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

# This variable is repository name
$REPOSITORY_NAME = $ARGV[1];

$REPOSITORY_PATH = $TOP_DIRECTORY . "/" . $REPOSITORY_NAME;


################################################################################
######################### BUNDLING FUNCTIONS ###################################
################################################################################

sub bundle_top_directory {
    my ($path) = @_;
    my $examples_directory = "$path/examples";
    my $error = 0;
    
    # if a system call does not return 0, there is an error
    $error += system "mkdir -p $path/bundles/$REPOSITORY_NAME";
    chdir "$path/bundles/";
    $error += system "cp -r $examples_directory/* ./$REPOSITORY_NAME";
    $error += system "zip -r " . $REPOSITORY_NAME . ".zip $REPOSITORY_NAME";
    $error += system "tar cvzf " . $REPOSITORY_NAME . 
                                                    ".tar.gz $REPOSITORY_NAME";
    $error += system "rm -rf $path/bundles/$REPOSITORY_NAME";
        
    # return to the top directory again
    chdir $TOP_DIRECTORY;
    
    #if error is not 0, there was error in the execution
    if ($error) {
        print "Error in bundle_top_directory function\n";
        exit ($error);
    }
}

sub bundle_each_example {
    my ($path)  = @_;

    my $exampes_path = "$path/examples/";
 
    opendir DIR, $exampes_path or die "ERROR trying to open $exampes_path $!\n";
    my @files = readdir DIR;
    
    close DIR;

    # we copy all the files to the repository_name folder
    system "cp -r * ./$REPOSITORY_NAME";
    system "rm -rf ./$REPOSITORY_NAME/$REPOSITORY_NAME";
   
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
        
        #if error is not 0, there was error in the execution
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

