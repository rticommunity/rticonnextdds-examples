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
/* filter.cs
 *
 * This file contains the functions needed by the Custom Content Filter to work.
 *
 * See the example README.txt file for details about each of these functions.
 *
 *  modification history
 *  ------------ -------
 *  2Mar2015,amb Example adapted for RTI Connext DDS 5.2
 */

using System;
public class custom_filter_type: DDS.IContentFilter {
    
    private interface evaluate_function {
        bool eval(long sample_data);
    }

    private class divide_test : evaluate_function {
        long _p = 1;

        public divide_test(long p) {
            _p = p;
        }

        public bool eval(long sample_data) {
            return (sample_data % _p == 0);
        }
    }

    private class gt_test : evaluate_function {
        long _p = 1;

        public gt_test(long p) {
            _p = p;
        }

        public bool eval(long sample_data) {
            return (sample_data > _p);
        }
    }

    /* Called when Custom Filter is created, or when parameters are changed */
    public void compile(ref object compile_data, string expression,
        DDS.StringSeq parameters, DDS.TypeCode type_code, 
        string type_class_name, object old_compile_data) {
        
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

        if (expression.StartsWith("%0 %1 ") && expression.Length > 6
            && parameters.length > 1) { // Enought parameters?
            long p = Convert.ToInt64(parameters.get_at(0));

            if (String.Compare(parameters.get_at(1), "greater-than") == 0) {
                compile_data = new gt_test(p);
                return;
            } else if (String.Compare(parameters.get_at(1), "divides") == 0) {
                compile_data = new divide_test(p);
                return;
            }
        }

        Console.WriteLine("CustomFilter: Unable to compile expresssion '"
            + expression + "'");
        Console.WriteLine("              with parameters '" + parameters.get_at(0)
            + "' '" + parameters.get_at(1) + "'");
        //throw (new DDS.Retcode_BadParameter());
    }
    
    /* Called to evaluated each sample */
    public bool evaluate(object compile_data, object sample, ref
        DDS.FilterSampleInfo meta_data) {
        long x = ((ccf) sample).x;
        return ((evaluate_function) compile_data).eval(x);
    }
    
    public void finalize(object compile_data) {
    }

};