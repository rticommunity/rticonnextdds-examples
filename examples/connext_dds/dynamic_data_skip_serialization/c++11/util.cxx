/*
 * (c) Copyright, Real-Time Innovations, 2023.  All rights reserved.
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the software solely for use with RTI Connext DDS. Licensee may
 * redistribute copies of the software provided that all such copies are subject
 * to this license. The software is provided "as is", with no warranty of any
 * type, including any warranty for fitness for any purpose. RTI is under no
 * obligation to maintain or support the software. RTI shall not be liable for
 * any incidental or consequential damages arising out of the use or inability
 * to use the software.
 */

#include "util.hpp"

namespace util {

void publish_example_data(
        unsigned int domain_id,
        const dds::core::xtypes::DynamicType &type)
{
    // Start communicating in a domain, usually one participant per application
    dds::domain::DomainParticipant participant(domain_id);

    // Create a Topic with a name and a datatype
    dds::topic::Topic<dds::core::xtypes::DynamicData> topic(
            participant,
            "Example Record",
            type);

    // Create a DataWriter with default QoS
    dds::pub::DataWriter<dds::core::xtypes::DynamicData> writer(
            dds::pub::Publisher(participant),
            topic);
    if (!wait_for_reader(writer)) {
        return;
    }

    rti::util::sleep(dds::core::Duration::from_secs(1));

    // Write a few data samples
    dds::core::xtypes::DynamicData data(type);
    data.value("id", std::string("Example"));

    std::vector<int32_t> payload;
    for (int i = 0; i < 10; i++) {
        payload.push_back(i);
    }

    data.set_values("payload", payload);
    writer.write(data);

    rti::util::sleep(dds::core::Duration::from_secs(1));

    writer.wait_for_acknowledgments(dds::core::Duration(5));
}

}  // namespace util

namespace application {
    bool shutdown_requested = false;

    void set_verbosity(
            rti::config::Verbosity & verbosity,
            int verbosity_value)
    {
        switch (verbosity_value) {
        case 0:
            verbosity = rti::config::Verbosity::SILENT;
            break;
        case 1:
            verbosity = rti::config::Verbosity::EXCEPTION;
            break;
        case 2:
            verbosity = rti::config::Verbosity::WARNING;
            break;
        case 3:
            verbosity = rti::config::Verbosity::STATUS_ALL;
            break;
        default:
            verbosity = rti::config::Verbosity::EXCEPTION;
            break;
        }
    }

    ApplicationArguments parse_arguments(int argc, char *argv[])
    {
        int arg_processing = 1;
        bool show_usage = false;
        ParseReturn parse_result = ParseReturn::ok;
        unsigned int domain_id = 0;
        unsigned int sample_count = (std::numeric_limits<unsigned int>::max)();
        ApplicationType app_type = ApplicationType::unknown;
        std::string file_name;
        rti::config::Verbosity verbosity(rti::config::Verbosity::EXCEPTION);

        while (arg_processing < argc) {
            if ((argc > arg_processing + 1)
                    && (strcmp(argv[arg_processing], "-d") == 0
                            || strcmp(argv[arg_processing], "--domain") == 0)) {
                domain_id = atoi(argv[arg_processing + 1]);
                arg_processing += 2;
            } else if (
                    (argc > arg_processing + 1)
                    && (strcmp(argv[arg_processing], "-s") == 0
                            || strcmp(argv[arg_processing], "--sample-count")
                                    == 0)) {
                sample_count = atoi(argv[arg_processing + 1]);
                arg_processing += 2;
            } else if (
                    (argc > arg_processing + 1)
                    && (strcmp(argv[arg_processing], "-v") == 0
                            || strcmp(argv[arg_processing], "--verbosity")
                                    == 0)) {
                set_verbosity(verbosity, atoi(argv[arg_processing + 1]));
                arg_processing += 2;
                // parse file_name
            } else if (
                    (argc > arg_processing + 1)
                    && (strcmp(argv[arg_processing], "--record") == 0)) {
                file_name = argv[arg_processing + 1];
                app_type = ApplicationType::record;
                arg_processing += 2;
            } else if (
                    (argc > arg_processing + 1)
                    && (strcmp(argv[arg_processing], "--replay") == 0)) {
                file_name = argv[arg_processing + 1];
                app_type = ApplicationType::replay;
                arg_processing += 2;
            } else if (strcmp(argv[arg_processing], "--publish") == 0) {
                app_type = ApplicationType::publish;
                arg_processing += 1;
            } else if (
                    strcmp(argv[arg_processing], "-h") == 0
                    || strcmp(argv[arg_processing], "--help") == 0) {
                std::cout << "Example application." << std::endl;
                show_usage = true;
                parse_result = ParseReturn::exit;
                break;
            } else {
                std::cout << "Bad parameter." << std::endl;
                show_usage = true;
                parse_result = ParseReturn::failure;
                break;
            }
        }

        if (app_type == ApplicationType::unknown) {
            std::cout << "Must specify --record, --replay, or --publish."
                      << std::endl;
            show_usage = true;
            parse_result = ParseReturn::failure;
        }

        if (show_usage) {
            std::cout
                    << "Options:\n"
                       "    --record <file name>       Record to a file.\n"
                       "    --replay <file name>       Replay from a file.\n"
                       "    --publish                  Publish example data.\n"
                       "    -d, --domain <int>         Domain ID this "
                       "application will\n"
                       "                               publish or subscribe "
                       "in.  \n"
                       "                               Default: 0\n"
                    << std::endl;
        }

        return ApplicationArguments(
                parse_result,
                domain_id,
                sample_count,
                app_type,
                file_name,
                verbosity);
    }
}