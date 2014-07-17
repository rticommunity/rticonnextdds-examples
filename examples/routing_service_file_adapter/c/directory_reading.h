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
/*
 * DirectoryReading.h
 */

#ifndef DIRECTORYREADING_H_
#define DIRECTORYREADING_H_

#define MAX_VEC_SIZE 256
#define MAX_NAME_SIZE 256

#include"data_structures.h"

/*Definition of the function implemented in the file DirectoryReading.c*/
void * RTI_RoutingServiceFileAdpater_checking_thread(void* arg);



#endif /* DIRECTORYREADING_H_ */
