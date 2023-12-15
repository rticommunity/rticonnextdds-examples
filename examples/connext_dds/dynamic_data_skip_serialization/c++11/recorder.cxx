

#include <fstream>
#include <iostream>
#include <string>
#include <rti/rti.hpp>

#include "util.hpp"

dds::core::xtypes::StructType create_type()
{
    using namespace dds::core::xtypes;

    StructType type("RecordExample", {
        Member("id", StringType(128)),
        Member("payload", SequenceType(primitive_type<int32_t>(), 1024))
    });

    return type;
}

void record(const std::string &file_name, int domain_id)
{
    using dds::core::xtypes::DynamicData;

    // To disable the deserialization on the DataReader and get direct access
    // to the serialized CDR buffer, we need to register the DataReader type
    // with the DynamicDataTypeSerializationProperty::skip_deserialization set
    // to true, and then create the DataReader with the name used to register
    // the type.
    dds::domain::DomainParticipant participant(domain_id);
    const std::string type_name = "RecordExample";
    rti::core::xtypes::DynamicDataTypeSerializationProperty property;
    property.skip_deserialization(true);
    rti::domain::register_type(participant, type_name, create_type(), property);

    dds::topic::Topic<DynamicData> topic(
            participant,
            "Example Record",
            type_name);  // specify the registered type name

    auto qos = dds::core::QosProvider::Default().datareader_qos(
            rti::core::builtin_profiles::qos_lib::generic_strict_reliable());
    dds::sub::DataReader<DynamicData> reader(topic, qos);
    if (!util::wait_for_writer(reader)) {
        return;
    }

    // File setup
    std::ofstream out_file(file_name, std::ios::binary);
    if (!out_file) {
        std::cerr << "Failed to open file for recording: " << file_name
                  << std::endl;
        return;
    }

    auto record_data = [&out_file, &reader]() {
        auto samples = reader.take();
        for (auto sample : samples) {
            if (!sample.info().valid()) {
                continue;
            }

            // Now the only way to access the data is to call get_cdr_buffer,
            // any other field accessor will fail.
            auto buffer_info = sample.data().get_cdr_buffer();
            auto buffer = buffer_info.first;
            auto buffer_length = buffer_info.second;
            std::cout << "Recording data sample (" << buffer_length << " bytes)"
                      << std::endl;
            out_file.write(
                    reinterpret_cast<const char *>(&buffer_length),
                    sizeof(buffer_length));
            out_file.write(
                    reinterpret_cast<const char *>(buffer),
                    buffer_length);
        }
    };

    // Set up a ReadCondition to trigger the record_data function when
    // data is available
    dds::core::cond::WaitSet waitset;
    dds::sub::cond::ReadCondition read_condition(
            reader,
            dds::sub::status::DataState::any(),
            record_data);
    waitset += read_condition;

    while (!application::shutdown_requested) {
        waitset.dispatch(dds::core::Duration(1));
    }
}

void replay(const std::string &file_name, int domain_id)
{
    using dds::core::xtypes::DynamicData;

    dds::domain::DomainParticipant participant(domain_id);

    // For the replay application we don't need to register the type with any
    // particular property
    dds::topic::Topic<DynamicData> topic(
            participant,
            "Example Record",
            create_type());

    auto qos = dds::core::QosProvider::Default().datawriter_qos(
            rti::core::builtin_profiles::qos_lib::generic_strict_reliable());
    dds::pub::DataWriter<DynamicData> writer(topic, qos);
    if (!util::wait_for_reader(writer)) {
        return;
    }

    std::ifstream in_file(file_name, std::ios::binary);
    if (!in_file) {
        std::cerr << "Failed to open file for replay: " << file_name
                  << std::endl;
        return;
    }

    uint32_t length;
    std::vector<char> buffer;
    DynamicData sample(create_type());
    while (!in_file.eof()) {
        // read length and data
        in_file.read(reinterpret_cast<char *>(&length), sizeof(length));

        std::cout << "Replaying data sample (" << length << " bytes)"
                  << std::endl;
        buffer.resize(length);
        in_file.read(buffer.data(), length);

        // By calling the set_cdr_buffer method we override the contents
        // of the DynamicData object with the new serialized data. After
        // setting a cdr buffer we can't use any field getters or setters.
        sample.set_cdr_buffer(buffer.data(), length);
        writer.write(sample);
    }

    in_file.close();
    writer.wait_for_acknowledgments(dds::core::Duration(10));
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
        if (arguments.application_type == ApplicationType::record) {
            record(arguments.file_name, arguments.domain_id);
        } else if (arguments.application_type == ApplicationType::replay) {
            replay(arguments.file_name, arguments.domain_id);
        } else if (arguments.application_type == ApplicationType::publish) {
            util::publish_example_data(arguments.domain_id, create_type());
        }

    } catch (const std::exception &ex) {
        std::cerr << "Exception in application: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    // Releases the memory used by the participant factory.  Optional at
    // application exit
    dds::domain::DomainParticipant::finalize_participant_factory();

    return EXIT_SUCCESS;
}
