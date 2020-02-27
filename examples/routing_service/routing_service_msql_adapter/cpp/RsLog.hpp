/*
 * (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software.  Licensee has the right to distribute object form
 * only for use with RTI products.  The Software is provided "as is", with no
 * warranty of any type, including any warranty for fitness for any purpose.
 * RTI is under no obligation to maintain or support the Software.  RTI shall
 * not be liable for any incidental or consequential damages arising out of the
 * use or inability to use the software.
 */
 
#ifndef RS_LOG_HPP_
#define RS_LOG_HPP_

/**
 * @internal
 */

#define RTI_RS_XSTR(x_)           #x_
#define RTI_RS_STRINGIFY(x_)      RTI_RS_XSTR(x_)

/*****************************************************************************
 *
*****************************************************************************/
#define RTI_RS_LOG_HEAD          "|| %s || I ||> "
#define RTI_RS_LOG_HEAD_WARNING  "|| %s || W ||> "
#define RTI_RS_LOG_HEAD_ERROR    "|| %s || E ||> "
#define RTI_RS_LOG_HEAD_TRACE    "|| %s || T ||> "

/*****************************************************************************
  
*****************************************************************************/

/* Error */
#define RTI_RS_ERROR(msg_) \
    fprintf(stdout,RTI_RS_LOG_HEAD_ERROR "%s\n", RTI_RS_LOG_ARGS, (msg_));

#if RTI_RS_USE_LOG

/* Log */
#define RTI_RS_LOG(msg_) \
    fprintf(stdout,RTI_RS_LOG_HEAD "%s\n", RTI_RS_LOG_ARGS, (msg_));

#define RTI_RS_LOG_FN(fn_) \
	RTI_RS_TRACE_1("CALL","%s",RTI_RS_STRINGIFY(fn_))

/* Trace */
#define RTI_RS_TRACE_1(msg_,fmt_,a1_) \
    fprintf(stdout,RTI_RS_LOG_HEAD_TRACE "%s " fmt_ "\n", RTI_RS_LOG_ARGS, (msg_), (a1_));

#else

#define RTI_RS_LOG(msg_)
#define RTI_RS_LOG_FN(fn_)
#define RTI_RS_TRACE_1(msg_, fmt_, a1_)

#endif  // RTI_RS_USE_LOG

#endif  // RS_LOG_HPP_