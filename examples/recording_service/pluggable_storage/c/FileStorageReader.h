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
 * Entry-point function to the C reader plugin. This function will create an
 * instance of the StorageReader example. This example opens a file passed in by
 * the configuration in the storage property tag. The file contains samples in
 * a textual format that is understood by the example StorageReader classes.
 * This example works exclusively with the type defined in the provided IDL
 * file, HelloMsg.idl.
 */
DLLExport
struct RTI_RecordingServiceStorageReader * FileStorageReader_create(
        const struct RTI_RoutingServiceProperties *properties);

/**
 * Function to delete a C reader plugin, previously created by calling
 * FileStorageReader_create().
 * This function should return any resources allocated by the creation function
 * and exit cleanly.
 */
DLLExport
void FileStorageReader_delete_instance(
        struct RTI_RecordingServiceStorageReader *storage_reader);
