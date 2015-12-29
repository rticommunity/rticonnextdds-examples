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

#include <dds/topic/Filter.hpp>
#include <rti/topic/ContentFilter.hpp>

//
// A generic ContentFilter that works with a lambda function.
//
// Since this filter is fully defined at compile time, it doesn't expect
// any expression or parameters.
//
template <typename T>
class LambdaFilter : public rti::topic::ContentFilter<T> {
public:

    // The expected function signature: takes a sample of type T and decides
    // whether it passes the filter (return true) or not (return false).
    typedef std::function<bool(const T&)> LambdaType;

    // Creates a LambdaFilter with the given filter function
    LambdaFilter(LambdaType f) : filter_(f) {}

    rti::topic::no_compile_data_t& compile(
       const std::string& expression,
       const dds::core::StringSeq& parameters,
       const dds::core::optional<dds::core::xtypes::DynamicType>& type_code,
       const std::string& type_class_name,
       rti::topic::no_compile_data_t* old_compile_data) override
    {
        // Nothing to compile, the lambda function does everything!
        return rti::topic::no_compile_data;
    }

    bool evaluate(
       rti::topic::no_compile_data_t& compile_data,
       const T& sample,
       const rti::topic::FilterSampleInfo& meta_data) override
    {
        // Run the function
        return filter_(sample);
    }

    void finalize(rti::topic::no_compile_data_t& compile_data) override
    {
        // Nothing to finalize
    }

private:
    LambdaType filter_; // The function used to filter
};

// Creates and register in the participant with the given name
// a new custom filter based on a lambda function.
//
// The filter can be used to create ContentFilteredTopics, QueryConditions or
// to enable DataWriters to perform writer-side
// filtering for DataReaders using a filter with the same name.
//
// @param name The name used to register the filter on the participant. This name
// is also used to allow a DataWriter to filter samples before sending them to
// a DataReader that uses a ContentFilteredTopic with the same filter. If the
// names are equal, the DataWriter will use his filter, so the lambda function
// should also be the same.
//
// @param participant The participant where to register the filter so its
// DataWriters and DataReaders can use it.
//
// @param lambda_func The function that decides if a sample of type T
// passes the filter or not. The function must have a single argument of type
// const T& and return a bool.
//
// @return A Filter object that can be used to create a ContentFilteredTopic,
// or a QueryCondition.
//
template <typename T>
dds::topic::Filter create_lambda_filter(
    const std::string& name,
    dds::domain::DomainParticipant participant,
    typename LambdaFilter<T>::LambdaType lambda_func)
{
    participant->register_contentfilter(
        rti::topic::CustomFilter<LambdaFilter<T> >(
            new LambdaFilter<T>(lambda_func)),
            name);

    dds::topic::Filter filter(""); // no expression
    filter->name(name);

    return filter;
}

// Uses the Filter that create_lambda_filter() returns to create
// a ContentFilteredTopic for the given Topic.
template <typename T>
dds::topic::ContentFilteredTopic<T> create_lambda_cft(
    const std::string& name,
    dds::topic::Topic<T>& topic,
    typename LambdaFilter<T>::LambdaType lambda_func)
{
    return dds::topic::ContentFilteredTopic<T>(
        topic,
        name,
        create_lambda_filter<T>(name, topic.participant(), lambda_func));
}
