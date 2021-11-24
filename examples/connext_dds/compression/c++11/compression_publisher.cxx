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

#include <iostream>
#include <fstream>
#include <vector>


#include <dds/pub/ddspub.hpp>
#include <rti/util/util.hpp>      // for sleep()
#include <rti/config/Logger.hpp>  // for logging
// alternatively, to include all the standard APIs:
//  <dds/dds.hpp>
// or to include both the standard APIs and extensions:
//  <rti/rti.hpp>
//
// For more information about the headers and namespaces, see:
//    https://community.rti.com/static/documentation/connext-dds/6.1.0/doc/api/connext_dds/api_cpp2/group__DDSNamespaceModule.html
// For information on how to use extensions, see:
//    https://community.rti.com/static/documentation/connext-dds/6.1.0/doc/api/connext_dds/api_cpp2/group__DDSCpp2Conventions.html

#include "application.hpp"  // for command line parsing and ctrl-c
#include "compression.hpp"

void run_publisher_application(
        unsigned int domain_id,
        unsigned int sample_count,
        const std::string &compression_id,
        const std::string &input_file)
{
    // DDS objects behave like shared pointers or value types
    // (see https://community.rti.com/best-practices/use-modern-c-types-correctly)

    // Start communicating in a domain, usually one participant per application
    dds::domain::DomainParticipant participant(domain_id);

    // Create a Topic with a name and a datatype
    dds::topic::Topic<StringLine> topic(participant, "Example StringLine");

    // Create a Publisher
    dds::pub::Publisher publisher(participant);

    dds::pub::qos::DataWriterQos datawriter_qos;

    if (compression_id.find("NONE") != std::string::npos) {
        datawriter_qos.policy<dds::core::policy::DataRepresentation>()
                .extensions()
                .compression_settings()
                .compression_ids(rti::core::CompressionIdMask(
                        DDS_COMPRESSION_ID_MASK_NONE));
    }
    if (compression_id.find("ZLIB") != std::string::npos) {
        datawriter_qos.policy<dds::core::policy::DataRepresentation>()
                .extensions()
                .compression_settings()
                .compression_ids(rti::core::CompressionIdMask(
                        DDS_COMPRESSION_ID_ZLIB_BIT));
    }
    if (compression_id.find("BZIP2") != std::string::npos) {
        datawriter_qos.policy<dds::core::policy::DataRepresentation>()
                .extensions()
                .compression_settings()
                .compression_ids(rti::core::CompressionIdMask(
                        DDS_COMPRESSION_ID_BZIP2_BIT));
    }
    if (compression_id.find("LZ4") != std::string::npos) {
        datawriter_qos.policy<dds::core::policy::DataRepresentation>()
                .extensions()
                .compression_settings()
                .compression_ids(rti::core::CompressionIdMask(
                        DDS_COMPRESSION_ID_LZ4_BIT));
    }

    // Create a DataWriter with default QoS
    dds::pub::DataWriter<StringLine> writer(publisher, topic);

    std::vector<StringLine> samples;
    if (!input_file.empty()) {
        // Open file to compress
        std::ifstream fileToCompress(input_file.c_str());
        if (!fileToCompress) {
            throw std::runtime_error("Fail opening input file");
        }
        std::string new_line;
        while (!std::getline(fileToCompress, new_line).eof()) {
            StringLine new_sample;
            new_sample.str(new_line);
            samples.push_back(new_sample);
        }
    } else {
        // Create a sample fill with 1024 zeros to send if no file has been
        // provided
        StringLine new_sample;
        new_sample.str(std::string(1024, '0'));
        samples.push_back(new_sample);
    }

    std::vector<StringLine>::iterator it = samples.begin();

    for (unsigned int samples_written = 0;
            !application::shutdown_requested && samples_written < sample_count;
            samples_written++, ++it) {

        /* Loop over the lines on the file */
        if (it==samples.end()) {
            it = samples.begin();
        }
        if (!(samples_written%10)) {
            std::cout << "Writing StringLine, count " << samples_written
                      << std::endl;
        }

        writer.write((*it));

        // Send once every 100 millisec
        rti::util::sleep(dds::core::Duration(0, 100000));
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
        run_publisher_application(
                arguments.domain_id,
                arguments.sample_count,
                arguments.compression_id,
                arguments.input_file);
    } catch (const std::exception& ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in run_publisher_application(): " << ex.what()
        << std::endl;
        return EXIT_FAILURE;
    }

    // Releases the memory used by the participant factory.  Optional at
    // application exit
    dds::domain::DomainParticipant::finalize_participant_factory();

    return EXIT_SUCCESS;
}
