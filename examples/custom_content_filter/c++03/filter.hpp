/*******************************************************************************
 (c) 2005-2015 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/

#include <cstdlib>
#include <dds/dds.hpp>
#include "ccf.hpp"

using namespace dds::core;
using namespace dds::core::xtypes;
using namespace rti::topic;

struct CustomCompileData {
    long param;
    bool (*eval_func)(long, long);
};

bool divide_test(long sample_data, long p) {
    return (sample_data % p == 0);
}

bool gt_test(long sample_data, long p) {
    return (p > sample_data);
}

class CustomFilterType: public ContentFilter<Foo, CustomCompileData> {
public:
    // Called when Custom Filter is created, or when parameters are changed.
    virtual CustomCompileData& compile(const std::string& expression,
        const StringSeq& parameters,
        const optional<DynamicType>& type_code,
        const std::string& type_class_name,
        CustomCompileData* old_compile_data)
    {
        // First free old data, if any.
        if (old_compile_data != NULL)
            delete old_compile_data;

        // We expect an expression of the form "%0 %1 <var>" where
        // %1 = "divides" or "greater-than" and <var> is an integral member of
        // the msg structure.
        // We don't actually check that <var> has the correct typecode,
        // (or even that it exists!). See example Typecodes to see
        // how to work with typecodes.
        // The compile information is a structure including the first filter
        // parameter (%0) and a function pointer to evaluate the sample

        // Check form.
        if (expression.compare(0, 6, "%0 %1 ") != 0) {
            throw std::invalid_argument("Invalid filter expression");
        }

        if (expression.size() < 7) {
            throw std::invalid_argument("Invalid filter expression size");
        }

        /* Check that we have params */
        if (parameters.size() < 2) {
            throw std::invalid_argument("Invalid filter parameters number");
        }

        CustomCompileData* cd = new CustomCompileData();
        cd->param = atol(parameters[0].c_str());

        if (parameters[1] == "greater-than") {
            cd->eval_func = &gt_test;
        } else if (parameters[1] == "divides") {
            cd->eval_func = &divide_test;
        } else {
            throw std::invalid_argument("Invad filter operation");
        }

        return *cd;
    }

    // Called to evaluated each sample.
    virtual bool evaluate(CustomCompileData& compile_data, const Foo& sample,
            const FilterSampleInfo& meta_data)
    {
        return compile_data.eval_func(sample.x(), compile_data.param);
    }

    virtual void finalize(CustomCompileData& compile_data)
    {
        // This is the pointer allocated in "compile" method.
        // For this reason we need to take the address.
        if (&compile_data != NULL)
            delete &compile_data;
    }
};
