#!/bin/bash

##############################################################################
# pull_from_github.sh
##############################################################################
# This script checks whether the examples repositories we have on github
# have been updated or not. If they have been updated, we bundle every 
# example in a zip and tar.gz file, and all the examples so they can be used
# on Drupal.
#
# Remember to follow the following steps for every repository you add from 
# git hub:
#
# 1) Clone example repository from github
# ---------------------------------------
# To checkout (clone) git repository:
# cd /path/to/where/you/want/git/repository
# git clone git://github.com/rticommunity/rticonnextdds-examples.git
# 
# 2) Configure remote repository
# ---------------------------
# Add github as a remote repository
# git remote add github git://github.com/rticommunity/rticonnextdds-examples.git
# 
# To pull new changes
# git pull github master #(or any other branch)
#
################################################################################

# Path to the directory where you have the examples, i.e., path to the directory
# where repositories have been cloned.
examples_path=/ssd/atlassian/installed/Bamboo/xml-data/build-dir/131073/RE-R63-SHIP

# List of repositories--separated by spaces--you want to bundle
repositories_list="rticonnextdds-examples"

# Note that the following parameters are commmon for all the repositories
# ---------------------------------------------------------------------------

# Remote repository origin name, i.e., name you gave to the remote repository
# when you run git remove add <remote_origin_name>
remote_origin_name="origin"

# Remote origin branch. By default it and it should always be master. 
remote_origin_branch="master"

# If you want to for re-bundling set force_rebundling to 1.
force_rebundling=0

# Iterate thorugh the repositories list looking for changes in each of 
# the repositories we have at github.
for repository_name in $repositories_list
do
    cd $examples_path/$repository_name
    
    # First we update our knowledge from the remote repository
    git remote update $remote_origin_name
    
    # Then we get the number of commits we are behind from the remote repository,
    # if this number is greater than 0, it means we need we need to run git pull 
    # and re-bundle the examples.
    commits=`git rev-list HEAD...${remote_origin_name}/${remote_origin_branch} --count`
    
    if [ $commits -gt 0 -o $force_rebundling -eq 1 ]; then
        git pull $remote_origin_name $remote_origin_branch
        #Bundle all the examples
        mkdir -p $examples_path/bundles/$repository_name
        cd $examples_path/bundles/$repository_name        
        cp -r $examples_path/$repository_name/examples $repository_name
        zip -r $repository_name.zip \
               $repository_name  
        tar cvzf $repository_name.tar.gz \
                 $repository_name
        rm -rf $repository_name
        #Rebundle each example 
        examples_list=`ls $examples_path/$repository_name/examples`
        for example in $examples_list
        do
            cd $examples_path/$repository_name/examples
            zip -r $example.zip $example
            tar cvzf $example.tar.gz $example
                # tar -C would be more elegant, but since zip doesn't have this 
                # option, we just cd to the folder.
                #tar cvzf $example.tar.gz -C $examples_path/$products_to_bundle-examples/examples $example
            mkdir -p $examples_path/$repository_name/bundles/$example
            mv $example.tar.gz $examples_path/$repository_name/bundles/$example/
            mv $example.zip $examples_path/$repository_name/bundles/$example/
        done
    fi
done
