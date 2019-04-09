# Example Code: Custom Content Filter

## Concept
*Custom Content Filters* are similar to the *Content Filtered Topic* (see
example. The main difference is that Custom Content Filters allow the use of
non-relational operations between topic members, i.e. a computation involving
topic members. Similarly to what happens with the Content Filtered Topics, after
applying a custom content filter you will get a subset of all samples that
fulfill an expression.

When creating your custom content filter, you can use two kinds of filter
expressions:

- **SQL sentence**: see the User Manual's section *SQL Filter Expression
  Notation* for further details.
- **String-match filter**: see the User Manual's section *STRINGMATCH Filter
  Expression Notation* for further details.

To create a Custom Content Filter we need to create three functions for the
three stages that it needs.

- **Compile function**: compiles a filter expression and parameters. *Connext*
  will call this function when a *ContentFilteredTopic* is created and when
  the filter parameters are changed. This parameter cannot be NULL.
  See *Compile Function* section in the User's Manual.

- **Evaluate function**: will be called by *Connext* each time a sample is
  received. Its purpose is to evaluate the sample based on the filter. Its
  parameter cannot be NULL. See *Evaluate Function* section in the User's
  Manual.

- **Finalize function**: will be called by *Connext* when an instance of the
  custom content filter is no longer needed. Its parameter may be NULL. See
  *Finalize Function* section in the User's Manual.

## Example Description
Our Custom Content Filter uses a *STRINGMATCH-based* filter. It takes the
expression `%0 %1 x`, where `%0` is an integer, `%1` is `divides` or
`greater-than`, and `x` is the field we are comparing in the sample. The Filter
lets the sample through if the *parameter[0]* divides or is greater than *x*,
respectively.

In this example, most of the work occurs in the compile stage. We check that we
have a valid expression, and initialize *compile_data* to a structure containing
the current parameter and a pointer to the evaluation function. When
`evaluate()` is called for each sample, it merely returns the result of that
function applied to the sample data.
