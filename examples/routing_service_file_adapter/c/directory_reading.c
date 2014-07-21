/*******************************************************************************
 (c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include"directory_reading.h"




/*
 * check if there is already a file with the same name inside the directory.
 * This function takes as a parameter the array containing the names of the
 * files previously discovered, the name of the file to check, and how many
 * entries we have in the array
 */
int RTI_RoutingServiceFileAdapter_is_file_present(
        char ** array_files,
        char * filename,
        int index)
{
    int i;

    if (index == 0) {
        return 0;
    }
    /*
     * This is inefficient, it could be done with a more efficient algorithm,
     * our focus now is on showing how RoutingService works
     */
    for (i = 0; i < index; i++) {

        if ((array_files[i] != NULL) && (!strcmp(filename,array_files[i]))) {
            return 1;
        }
    }
    return 0;
}


/*
 * Inside this function we write the code that we want to be executed every time
 * This thread discover a new file in the scanned folder. For now we call
 * on_data_available on the discovery listener and we print that a new
 * file has been discovered
 */
void RTI_RoutingServiceFileAdapter_send_event(
        char* fname,
        struct RTI_RoutingServiceFileConnection *connection)
{

    connection->input_discovery_listener.on_data_available(
            connection->input_discovery_reader,
            connection->input_discovery_listener.listener_data);

    fprintf(stdout,"checkingThread: new file discovered,"
            " new stream will be created:%s\n",fname);
}

/*
 * The core execution of the thread that every five seconds checks
 * if there are new files in the directory
 */
void * RTI_RoutingServiceFileAdpater_checking_thread(
        void* arg)
{

    int i,index = 0;
    int count = 0;
    char fname[MAX_NAME_SIZE];
    char dirpath[MAX_NAME_SIZE];    /*when we use a struct stat element, we need
     	 	 	 	 	 	 	 	 the full path to pass as a parameter to
     	 	 	 	 	 	 	 	 know if it is a file or directory*/
    char ** array_files = NULL;
    struct RTI_RoutingServiceFileConnection * connection =
            ( struct RTI_RoutingServiceFileConnection *) arg;
    struct dirent * directory_info = NULL;
    DIR* dir = NULL;
    struct stat dir_stat;/*we use it for getting information about the file
                            we have already read*/

    /*
     * initialize the array that is going to contain the
     * discovery information with the names of the files inside the
     * directory. the array will be constantly increasing.
     * We don't notify the Routing Service in case something goes wrong, we just
     * print a warning on stderr when executing look at the warning if nothing
     * happens in the Routing Service have look at the warning received.
     */
    array_files = (char**)malloc(MAX_VEC_SIZE*sizeof(char*));
    if (array_files == NULL ) {
        fprintf(stderr,"checkingThread: error allocating memory\n");
        return NULL;
    }

    /*
     * we assign to the discoveryData pointer inside the structure
     * stream_reader, the pointer to array_files, which contains the list of the
     * files inside the  input folder
     */

    connection->input_discovery_reader->discovery_data = array_files;
    dir = opendir(connection->path);
    if (dir == NULL) {
        fprintf(stderr,"checkingThread: error opening directory");
    }

    /*every 5 seconds we check if there are new files in the directory*/
    while (connection->is_running_enabled) {
        sleep(connection->sleep_period);

        while ((directory_info = readdir(dir)) != NULL) {
            strcpy(fname,directory_info->d_name);
            sprintf(dirpath,"%s/%s",connection->path,fname);
            stat(dirpath,&dir_stat);
            /*
             * when we read a name inside the directory
             * if it is not present inside the array array_files
             * we put the name file in the next free position.
             * We also check that it is a file and not a directory, as we
             * don't copy directory to destination, just files, we use stat
             * function for getting that informations
             */
            if ((fname[0] != '.')&&(!S_ISDIR(dir_stat.st_mode))) {
                /* we don't consider hidden files and directories */
                /* Then we check if the file is already in our array */
                if (!RTI_RoutingServiceFileAdapter_is_file_present(
                        array_files,fname,index)) {
                    array_files[index] = (char*)malloc(MAX_NAME_SIZE);
                    if (array_files[index] == NULL) {
                        fprintf(stderr,"checkingThread:"
                                "Error allocating memory for discovery array");
                        continue;
                    }

                    strcpy(array_files[index], fname);

                    /*
                     * in this example to take it easy we set a maximum
                     * number of file we can discover. That number is
                     * defined by the MAX_VEC_SIZE macro. If you prefer
                     * you can increase it.
                     */
                    if (index < MAX_VEC_SIZE) {
                        RTI_RoutingServiceFileAdapter_send_event(
                                fname,connection);
                        index++;
                    }
                    else {
                        fprintf(stdout,"checkingThread: You reached the "
                                "maximum number of file in the directory "
                                "you can have!\n RoutingService no longer "
                                "gets notified for creating new streams\n"
                                "For increasing number of files, increase "
                                "the size of MAX_VEC_FILE in the header "
                                "file ");
                    }
                    connection->input_discovery_reader->
                    discovery_data_counter = index;
                }
            }
        }
        /*
         * now we reset the position of the directory stream
         * to the beginning of the directory
         */
        rewinddir(dir);
    }
    closedir(dir);
    fprintf(stdout,"checkingThread: directory closed\n");
    for (i = 0; i < index; i++) {
        free(array_files[i]);
    }
    free(array_files);

    pthread_exit(NULL);
    return NULL;/*just because the compiler wants a return*/

}

