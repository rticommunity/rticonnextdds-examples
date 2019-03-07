/*
 * (c) 2019 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software.  Licensee has the right to distribute object form
 * only for use with RTI products.  The Software is provided "as is", with no
 * warranty of any type, including any warranty for fitness for any purpose.
 * RTI is under no obligation to maintain or support the Software.  RTI shall
 * not be liable for any incidental or consequential damages arising out of the
 * use or inability to use the software.
 */

#include "routingservice/routingservice_infrastructure.h"

/**
 * Entry-point function to the C reader plugin. This function will create an
 * instance of the StorageReader example. This example opens a file passed in by
 * the configuration in the storage property tag. The file contains samples in
 * a textual format that is understood by the example StorageReader classes.
 * This example works exclusively with the type defined in the provided IDL
 * file, HelloMsg.idl.
 */
RTI_USER_DLL_EXPORT
struct RTI_RecordingServiceStorageReader * FileStorageReader_create(
        const struct RTI_RoutingServiceProperties *properties);

/**
 * Function to delete a C reader plugin, previously created by calling
 * FileStorageReader_create().
 * This function should return any resources allocated by the creation function
 * and exit cleanly.
 */
RTI_USER_DLL_EXPORT
void FileStorageReader_delete_instance(
        struct RTI_RecordingServiceStorageReader *storage_reader);
