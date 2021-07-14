/*******************************************************************************
 (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/

#include <csignal>
#include <dds/dds.hpp>
#include <rti/util/util.hpp>  // for sleep()

const std::string QOS_URL = "file://application.xml";
const std::string PARTICIPANT_NAME = "domain_participant_library::participant";
const std::string WRITER_NAME = "publisher::writer";
const std::string READER_NAME = "subscriber::reader";
const std::string KVP_TYPE_NAME = "KeyValuePair";

// Catch control-C and tell application to shut down
bool shutdown_requested = false;

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

class MyDataReaderListener : public dds::sub::NoOpDataReaderListener<
                                     dds::core::xtypes::DynamicData> {
    virtual void on_data_available(
            dds::sub::DataReader<dds::core::xtypes::DynamicData> &reader)
    {
        dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> samples =
                reader.take();
        for (const auto &sample : samples) {
            // If the reference we get is valid data, it means we have actual
            // data available, otherwise we received metadata.
            if (sample.info().valid()) {
                std::cout << sample.data() << std::endl;
            } else {
                std::cout << "  Received metadata" << std::endl;
            }
        }
    }
};

// Writes environment variables to a DDS topic.  Each environment variable is
// written as a key/value pair to the DDS bus.
//
//  writer - a DataWriter for DnyamicData
//  kvpType - a key value pair DynamicType
//  env - the environment array
void publish_env(
        int id,
        dds::pub::DataWriter<dds::core::xtypes::DynamicData> &writer,
        const dds::core::xtypes::DynamicType &kvpType,
        char *env[])
{
    // Create one sample from the specified type and populate the id field.
    // This sample will be used repeatedly in the loop below.
    dds::core::xtypes::DynamicData sample(kvpType);
    sample.value<int64_t>("id", id);

    // Loop over the env pointer, extract the key and value from the enviroment
    // variable, set the values in the sample, and write with the writer.
    char buf[4096];
    while (*env) {
        try {
            strncpy(buf, *env, 4095);
            buf[4095] = '\0';  // prevent buffer overflow
            char *key = strtok(buf, "=");
            char *value = strtok(NULL, "=");
            sample.value<std::string>("key", key);
            sample.value<std::string>("value", value);
            env++;

            // Now we can write out the populated sample.
            writer.write(sample);
        } catch (std::exception ex) {
            std::cout << "Exception caught publishing sample: " << ex.what()
                      << std::endl;
        }
    }
}


int main(int argc, char *argv[], char *envp[])
{
    setup_signal_handlers();

    // Create the QosPovider from the specified xml file.
    // The application.xml filename is used in this example instead of the
    // default.
    dds::core::QosProvider qos_provider(QOS_URL);

    // Create the participant as defined in the xml file.  This instantiats the
    // participant registeres the types, and creates the child publisher,
    // subscriber, reader and writer.
    dds::domain::DomainParticipant participant =
            qos_provider->create_participant_from_config(PARTICIPANT_NAME);

    // Lookup the specific KeyValuePair type as defined in the xml file.
    // This will be needed to create samples of the correct type when
    // publishing.
    const dds::core::xtypes::DynamicType &myType =
            qos_provider->type(KVP_TYPE_NAME);

    // Find the DataWriter defined in the xml by using the participant and the
    // publisher::writer pair as the datawriter name.
    dds::pub::DataWriter<dds::core::xtypes::DynamicData> writer =
            rti::pub::find_datawriter_by_name<
                    dds::pub::DataWriter<dds::core::xtypes::DynamicData>>(
                    participant,
                    WRITER_NAME);

    // Find the DataReader defined in the xml by using the participant and the
    // subscriber::reader pair as the datareader name.
    dds::sub::DataReader<dds::core::xtypes::DynamicData> reader =
            rti::sub::find_datareader_by_name<
                    dds::sub::DataReader<dds::core::xtypes::DynamicData>>(
                    participant,
                    READER_NAME);

    // Create a shared pointer for MyDataReaderListener
    auto my_listener = std::make_shared<MyDataReaderListener>();

    // Associate the listener to the data reader.
    reader.set_listener(my_listener);

    // Ignore our own publications so they do not trigger our data listener.
    dds::domain::ignore(participant, participant.instance_handle());

    // Application id to uniquely identify data from a particular application
    int id = std::rand();

    // Publish the environment vairables to the topic configured in the
    // xml writer.
    while (!shutdown_requested) {
        std::cout << "Publishing Environment" << std::endl;
        publish_env(id, writer, myType, envp);
        rti::util::sleep(dds::core::Duration(2));
    }

    return 0;
}
