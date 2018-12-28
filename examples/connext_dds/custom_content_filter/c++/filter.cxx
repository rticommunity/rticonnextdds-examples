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
/* filter.cxx
 *
 * This file contains the functions needed by the Custom Content Filter to work.
 *
 * See the example README.txt file for details about each of these functions.
 *
 *  modification history
 *  ------------ -------
 *  21May2014,amb Example adapted for RTI Connext DDS 5.1
 */

class custom_filter_type: public DDSContentFilter {
public:
    virtual DDS_ReturnCode_t compile(void** new_compile_data,
            const char *expression, const DDS_StringSeq& parameters,
            const DDS_TypeCode* type_code, const char* type_class_name,
            void *old_compile_data);

    virtual DDS_Boolean evaluate(void* compile_data, const void* sample,
            const struct DDS_FilterSampleInfo * meta_data);

    virtual void finalize(void* compile_data);
};

bool divide_test(long sample_data, long p) {
    return (sample_data % p == 0);
}

bool gt_test(long sample_data, long p) {
    return (p > sample_data);
}

/* Custom compile data */
struct cdata {
    long param;
    bool (*eval_func)(long, long);
};

/* Called when Custom Filter is created, or when parameters are changed */
DDS_ReturnCode_t custom_filter_type::compile(void** new_compile_data,
        const char *expression, const DDS_StringSeq& parameters,
        const DDS_TypeCode* type_code, const char* type_class_name,
        void *old_compile_data) {
    struct cdata* cd = NULL;
    
    /* First free old data, if any */
    delete old_compile_data;

    /* We expect an expression of the form "%0 %1 <var>"
     * where %1 = "divides" or "greater-than"
     * and <var> is an integral member of the msg structure.
     * 
     * We don't actually check that <var> has the correct typecode,
     * (or even that it exists!). See example Typecodes to see 
     * how to work with typecodes.
     *
     * The compile information is a structure including the first filter
     * parameter (%0) and a function pointer to evaluate the sample
     */

    /* Check form: */
    if (strncmp(expression, "%0 %1 ", 6) != 0) {
        goto err;
    }

    if (strlen(expression) < 7) {
        goto err;
    }

    /* Check that we have params */
    if (parameters.length() < 2) {
        goto err;
    }

    cd = (struct cdata*) malloc(sizeof(struct cdata));
    sscanf(parameters[0], "%ld", &cd->param);

    if (strcmp(parameters[1], "greater-than") == 0) {
        cd->eval_func = gt_test;
    } else if (strcmp(parameters[1], "divides") == 0) {
        cd->eval_func = divide_test;
    } else {
        goto err;
    }

    *new_compile_data = cd;
    return DDS_RETCODE_OK;

err:
    printf("CustomFilter: Unable to compile expression '%s'\n",
            expression);
    printf("              with parameters '%s' '%s'\n", parameters[0],
            parameters[1]);
    *new_compile_data = NULL;
    return DDS_RETCODE_BAD_PARAMETER;
}

/* Called to evaluated each sample */
DDS_Boolean custom_filter_type::evaluate(void* compile_data, const void* sample,
        const struct DDS_FilterSampleInfo * meta_data) {
    cdata* cd = (cdata*) compile_data;
    ccf* msg = (ccf*) sample;

    if (cd->eval_func(msg->x, cd->param))
        return DDS_BOOLEAN_TRUE;
    else
        return DDS_BOOLEAN_FALSE;
}

void custom_filter_type::finalize(void* compile_data) {
    if (compile_data != NULL)
        delete compile_data;
}

