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
/* CustomFilter.java
 *
 * This file contains the functions needed by the Custom Content Filter to work.
 *
 * See the example README.txt file for details about each of these functions.
 *
 *  modification history
 *  ------------ -------
 *  21May2014,amb Example adapted for RTI Connext DDS 5.1
 */


import com.rti.dds.domain.*;
import com.rti.dds.infrastructure.*;
import com.rti.dds.publication.*;
import com.rti.dds.topic.*;
import com.rti.dds.typecode.*;

// Called when Custom Filter is created, or when parameteres are changed
public class CustomFilter implements ContentFilter {

    // Functor interface
    private interface EvaluateFunction {
        public boolean eval(long sample_data);
    };

    private class DividesTest implements EvaluateFunction {
        long _p = 1;

        public DividesTest(long p) {
            _p = p;
        }

        public boolean eval(long sample_data) {
            return (sample_data % _p == 0);
        }
    };

    private class GreaterThanTest implements EvaluateFunction {
        long _p = 1;

        public GreaterThanTest(long p) {
            _p = p;
        }

        public boolean eval(long sample_data) {
            return (_p > sample_data);
        }
    };

    public void compile(ObjectHolder new_compile_data, String expression,
            StringSeq parameters, TypeCode type_code, String type_class_name,
            Object old_compile_data) {
        /*
         * We expect an expression of the form "%0 %1 <var>" where %1 =
         * "divides" or "greater-than" and <var> is an integral member of the
         * msg structure.
         * 
         * We don't actually check that <var> has the correct typecode, (or even
         * that it exists!). See example Typecodes to see how to work with
         * typecodes.
         * 
         * The compile information is a structure including the first filter
         * parameter (%0) and a function pointer to evaluate the sample
         */

        // Check form:
        if (expression.startsWith("%0 %1 ") && expression.length() > 6
                && parameters.size() > 1) { // Enough parameters?

            long p = Long.valueOf((String) parameters.get(0)).longValue();

            if (parameters.get(1).equals("greater-than")) {
                new_compile_data.value = new GreaterThanTest(p);
                return;
            } else if (parameters.get(1).equals("divides")) {
                new_compile_data.value = new DividesTest(p);
                return;
            }
        }

        System.out.print("CustomFilter: Unable to compile expression '"
                + expression + "'\n");
        System.out.print("              with parameters '" + parameters.get(0)
                + "' '" + parameters.get(1) + "'\n");
        throw (new RETCODE_BAD_PARAMETER());
    }

    public boolean evaluate(Object compile_data, Object sample,
            FilterSampleInfo meta_data) {
        long x = ((ccf) sample).x;
        return ((EvaluateFunction) compile_data).eval(x);
    }

    public void finalize(Object compile_data) {
    }
};
