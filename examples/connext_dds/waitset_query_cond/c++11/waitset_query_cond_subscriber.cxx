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

#include <dds/sub/ddssub.hpp>
#include <dds/core/ddscore.hpp>
#include <rti/config/Logger.hpp>  // for logging

#include "waitset_query_cond.hpp"
#include "application.hpp"  // for command line parsing and ctrl-c

void run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // Create a DomainParticipant.
    dds::domain::DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type.
    dds::topic::Topic<waitset_query_cond> topic(
            participant,
            "Example waitset_query_cond");

    // Retrieve the DataReader QoS, from USER_QOS_PROFILES.xml
    dds::sub::qos::DataReaderQos reader_qos =
            dds::core::QosProvider::Default().datareader_qos();

    // If you want to change the DataReader's QoS programmatically rather
    // than using the XML file, you will need to uncomment the following lines.

    // reader_qos << History::KeepLast(1);

    // Create a subscriber
    dds::sub::Subscriber subscriber(participant);

    // Create a DataReader.
    dds::sub::DataReader<waitset_query_cond> reader(
            subscriber,
            topic,
            reader_qos);

    // Create the parameters of the condition.
    // The initial value of the parameter is EVEN string to get even numbers.
    std::vector<std::string> query_parameters = { "'EVEN'" };

    int samples_read = 0;
    // Create a query condition with a functor handler.
    dds::sub::cond::QueryCondition query_condition(
            dds::sub::Query(reader, "name MATCH %0", query_parameters),
            dds::sub::status::DataState::any_data(),
            [&reader, &samples_read](dds::core::cond::Condition condition) {
                // We take only samples that triggered the QueryCondition
                auto condition_as_qc = dds::core::polymorphic_cast<
                        dds::sub::cond::QueryCondition>(condition);
                dds::sub::LoanedSamples<waitset_query_cond> samples =
                        reader.select().condition(condition_as_qc).take();
                for (auto sample : samples) {
                    if (!sample.info().valid()) {
                        std::cout << "Got metadata" << std::endl;
                    } else {
                        std::cout << sample.data() << std::endl;
                        samples_read++;
                    }
                }
            });

    // Create the waitset and attach the condition.
    dds::core::cond::WaitSet waitset;
    waitset += query_condition;

    dds::core::Duration wait_timeout(1);
    std::cout << std::endl
              << ">>> Timeout: " << wait_timeout.sec() << " sec" << std::endl
              << ">>> Query conditions: " << query_condition.expression()
              << std::endl
              << "\t %%0 = " << query_parameters[0] << std::endl
              << "----------------------------------" << std::endl
              << std::endl;

    bool condition_changed = false;

    while (!application::shutdown_requested && samples_read < sample_count) {
        // We change the parameter in the Query Condition after 7 secs.
        if (samples_read == 7 && !condition_changed) {
            query_parameters[0] = "'ODD'";
            query_condition.parameters(
                    query_parameters.begin(),
                    query_parameters.end());

            std::cout << std::endl
                      << "CHANGING THE QUERY CONDITION" << std::endl
                      << ">>> Query conditions: "
                      << query_condition.expression() << std::endl
                      << "\t %%0 = " << query_parameters[0] << std::endl
                      << ">>> We keep one sample in the history" << std::endl
                      << "-------------------------------------" << std::endl
                      << std::endl;

            condition_changed = true;
        }

        // 'dispatch()' blocks execution of the thread until one or more
        // attached Conditions become true, or until a user-specified timeout
        // expires and then dispatches the events.
        // Another way would be to use 'wait()' and check if the QueryCondition
        // has been triggered as the "waitsets" example does.
        waitset.dispatch(wait_timeout);
    }
}

int main(int argc, char *argv[])
{
    using namespace application;

    // Parse arguments and handle control-C
    auto arguments = parse_arguments(argc, argv);
    if (arguments.parse_result == ParseReturn::exit) {
        return EXIT_SUCCESS;
    } else if (arguments.parse_result == ParseReturn::failure) {
        return EXIT_FAILURE;
    }
    setup_signal_handlers();

    // Sets Connext verbosity to help debugging
    rti::config::Logger::instance().verbosity(arguments.verbosity);

    try {
        run_subscriber_application(arguments.domain_id, arguments.sample_count);
    } catch (const std::exception &ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in run_subscriber_application(): " << ex.what()
                  << std::endl;
        return EXIT_FAILURE;
    }

    // Releases the memory used by the participant factory.  Optional at
    // application exit
    dds::domain::DomainParticipant::finalize_participant_factory();

    return EXIT_SUCCESS;
}
