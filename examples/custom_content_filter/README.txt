===========================================
 Example Code -- Custom Content Filter
===========================================

Concept
-------
A Custom Content Filter is similar to the filters on Topics, see example
'Content Filtered Topic'. However the main difference is that you can use
filters non-bases on relational operations between topic members, not on a 
computation involving topic members. In this way, you will get a subset of all 
samples controller by an expression. You have two expression kind to create your
own custom content filter.

    - A SQL sentence, you can see the User Manual's section 5.4.6 "SQL Filter 
      Expression Notation" for further details.
    - A String matcher filter, you can see the User Manual's 5.4.7 "STRINGMATCH
      Filter Expression Notation" for futher details.


Example Description
-------------------
PUBLISHER: 
As filtering can also occur on the publishing side, we register the custom 
content filter here as well. 

SUBSCRIBER: 
This shows how to use a custom content filter. For information on normal 
filters, see example 'Content Filtered Topic'. 

We will use a STRINGMATCH as Custom Content Filter, which takes the expression 
"%0 %1 x", where %0 is an integer, %1 is "divides" or "greater-than", and x is
the field we are comparing in the sample. The Filter lets the sample through if
the parameter[0] divides or is greater than x, respectively. 

To create a Custom Content Filter we need to create three functions for the
three stages that it needs.
    - Compile function: The function that will be used to compile a filter 
      expression and parameters. Connext will call this function when a 
      ContentFilteredTopic is created and when the filter parameters are 
      changed. This parameter cannot be NULL. See "Compile Function" 
      (Section 5.4.8.5).
      
    - Evaluate function: The function that will be called by Connext each time a 
      sample is received. Its purpose is to evaluate the sample based on the 
      filter. This parameter cannot be NULL. See "Evaluate Function" 
      (Section 5.4.8.6).

    - Finalize function: The function that will be called by Connext when an
      instance of the custom content filter is no longer needed. This parameter
      may be NULL. See "Finalize Function" (Section 5.4.8.7). 
      
In this example of the work occurs in the compile stage. We check that we have
a valid expression, and initialize compile_data to a structure containing the
current parameter and a pointer to the evaluation function. When evaluate() is
called for each sample, it merely returns the result of that function applied to
the sample data. 
