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
 * Entry-point function to the C storage plugin. This method will create an
 * instance of the StorageWriter example.
 * When a valid stream is found, a StorageStreamWriter instance will be created
 * attached to that stream that is able to store the data in the file passed in
 * by the configuration in the storage property tag. Then for every sample that
 * it receives it stores a text entry with the sample number, the count, the
 * reception timestamp, the valid data flag and if the latter is true, then the
 * sample's data. This example works exclusively with the type defined in the
 * provided IDL file, HelloMsg.idl.
 */
RTI_USER_DLL_EXPORT
struct RTI_RecordingServiceStorageWriter *
FileStorageWriter_create_instance(
        const struct RTI_RoutingServiceProperties *properties);

/**
 * This function deletes a StorageWriter instance that has been created by this
 * plugin. All allocated resources should be freed.
 */
RTI_USER_DLL_EXPORT
void FileStorageWriter_delete_instance(
        struct RTI_RecordingServiceStorageWriter *storage_writer);
