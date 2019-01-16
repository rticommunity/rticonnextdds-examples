/*
 * (c) Copyright, Real-Time Innovations, 2018-.
 * All rights reserved.
 * No duplications, whole or partial, manual or electronic, may be made
 * without express written permission.  Any such copies, or
 * revisions thereof, must display this notice unaltered.
 * This code contains trade secrets of Real-Time Innovations, Inc.
 */

#include "routingservice/routingservice_infrastructure.h"

#ifdef _WIN32
#define DLLExport __declspec(dllexport)
#else
#define DLLExport 
#endif

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
DLLExport
struct RTI_RecordingServiceStorageWriter *
FileStorageWriter_create_instance(
        const struct RTI_RoutingServiceProperties *properties);

/**
 * This function deletes a StorageWriter instance that has been created by this
 * plugin. All allocated resources should be freed.
 */
DLLExport
void FileStorageWriter_delete_instance(
        struct RTI_RecordingServiceStorageWriter *storage_writer);
