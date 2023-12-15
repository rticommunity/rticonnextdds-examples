/*
 * (c) Copyright, Real-Time Innovations, 2020.  All rights reserved.
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the software solely for use with RTI Connext DDS. Licensee may
 * redistribute copies of the software provided that all such copies are subject
 * to this license. The software is provided "as is", with no warranty of any
 * type, including any warranty for fitness for any purpose. RTI is under no
 * obligation to maintain or support the software. RTI shall not be liable for
 * any incidental or consequential damages arising out of the use or inability
 * to use the software.
 */

#ifndef APPLICATION_HPP
#define APPLICATION_HPP


#include <iostream>
#include <string>
#include <csignal>
#include <dds/core/ddscore.hpp>
#include <dds/pub/ddspub.hpp>
#include <dds/sub/ddssub.hpp>
#include <rti/util/util.hpp>  // for sleep()


namespace application
{
    // Catch control-C and tell application to shut down
    extern bool shutdown_requested;

    inline void stop_handler(int)
    {
        shutdown_requested = true;
        std::cout << "preparing to shut down..." << std::endl;
    }

    inline void setup_signal_handlers()
    {
        signal(SIGINT, stop_handler);
        signal(SIGTERM, stop_handler);
    }

    enum class ParseReturn { ok, failure, exit };

    enum class ApplicationType { unknown, record, replay, publish };

    struct ApplicationArguments {
        ParseReturn parse_result;
        unsigned int domain_id;
        unsigned int sample_count;
        ApplicationType application_type = ApplicationType::unknown;
        std::string file_name;
        rti::config::Verbosity verbosity;

        ApplicationArguments(
                ParseReturn parse_result_param,
                unsigned int domain_id_param,
                unsigned int sample_count_param,
                ApplicationType application_type_param,
                const std::string &file_name_param,
                rti::config::Verbosity verbosity_param)
                : parse_result(parse_result_param),
                  domain_id(domain_id_param),
                  sample_count(sample_count_param),
                  application_type(application_type_param),
                  file_name(file_name_param),
                  verbosity(verbosity_param)
        {
        }
    };

    inline void set_verbosity(
            rti::config::Verbosity & verbosity,
            int verbosity_value);

    // Parses application arguments for example.
    ApplicationArguments parse_arguments(int argc, char *argv[]);

}  // namespace application

namespace util {

template<typename T>
bool wait_for_writer(const dds::sub::DataReader<T> &reader)
{
    std::cout << "Waiting for a matching DataWriter..." << std::endl;
    while (!application::shutdown_requested
            && dds::sub::matched_publications(reader).empty()) {
        rti::util::sleep(dds::core::Duration::from_millisecs(100));
    }

    if (application::shutdown_requested) {
        return false;
    }

    std::cout << "DataWriter found" << std::endl;
    return true;
}

template<typename T>
bool wait_for_reader(const dds::pub::DataWriter<T> &writer)
{
    std::cout << "Waiting for a matching DataReader..." << std::endl;
    while (!application::shutdown_requested
            && dds::pub::matched_subscriptions(writer).empty()) {
        rti::util::sleep(dds::core::Duration::from_millisecs(100));
    }

    if (application::shutdown_requested) {
        return false;
    }

    std::cout << "DataReader found" << std::endl;
    return true;
}

void publish_example_data(
        unsigned int domain_id,
        const dds::core::xtypes::DynamicType &type);

}  // namespace util

#endif  // APPLICATION_HPP
