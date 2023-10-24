//
// (c) 2023 Copyright, Real-Time Innovations, Inc.  All rights reserved.
//
//  RTI grants Licensee a license to use, modify, compile, and create derivative
//  works of the Software.  Licensee has the right to distribute object form
//  only for use with RTI products.  The Software is provided "as is", with no
//  warranty of any type, including any warranty for fitness for any purpose.
//  RTI is under no obligation to maintain or support the Software.  RTI shall
//  not be liable for any incidental or consequential damages arising out of the
//  use or inability to use the software.
//

#include <iostream>

#include <rti/util/util.hpp> // for rti::util::sleep()
#include <rti/distlogger/DistLogger.hpp>

using namespace std;
using namespace dds::domain;
using namespace rti::dist_logger;


void hello_main(
        const string application_kind,
        const int domain_id,
        const int sleep,
        const uint iterations)
{
    // First, create the options to personalize Distributed Logger.
    // If no options are provided, default ones will be created.
    DistLoggerOptions options;
    options.domain_id(domain_id);
    options.application_kind(application_kind);

    // Then, set the created options.
    // You can only call set_options before getting the Distributed Logger
    // instance. Once an instance has been created, attempting to call
    // set_options will throw an exception.
    DistLogger::set_options(options);

    // Instantiate Distributed Logger
    DistLogger dist_logger = DistLogger::get_instance();
    
    // RTI Distributed Logger provides the ability to interact with its
    // topics directly. However, for the sake of simplicity in this example,
    // you may use RTI Tools such as RTI Spy or RTI Admin Console to visualize
    // the logging.
    for (uint i = 1; i <= iterations; ++i) {
        cout << "\nIteration #" << i << endl;

        // Log messages using the appropiate log levels for your messages.
        dist_logger.debug("This is a debug message");
        dist_logger.warning("This is a warning message");
        dist_logger.error("This is an error message");

        rti::util::sleep(dds::core::Duration(sleep));
    }

    // The DistLogger instance must be finalized for clean-up
    // before the the participant factory is finalized.
    DistLogger::finalize();
}


int main(int argc, char *argv[])
{
    const string application = "Modern C++ Distributed Logger Example";

    int domain_id = 0;
    int sleep = 1;
    uint iterations = 50;

    for (int i = 1; i < argc;) {
        const string &param = argv[i++];

        if ((param == "-d" || param == "--domain_id") && i < argc) {
            domain_id = atoi(argv[i++]);
        } else if ((param == "-i" || param == "--iterations") && i < argc) {
            iterations = atoi(argv[i++]);
        } else if ((param == "-s" || param == "--sleep") && i < argc) {
            sleep = atoi(argv[i++]);
        } else {
            cout << argv[0]
                 << " [options]\n"
                 << "\t-d, --domain_id <domain ID> (default: 0)\n"
                 << "\t-s, --sleep <seconds between iterations> (default: 1)\n"
                 << "\t-i, --iterations <number of iterations> (default: 50)\n"
                 << endl;
            return -1;
        }
    }

    try {
        hello_main(application, domain_id, sleep, iterations);
        
        // Finalize Domain Participant Factory
        dds::domain::DomainParticipant::finalize_participant_factory();
    } catch (const exception &ex) {
        // This will catch DDS exceptions
        cerr << "Exception in hello_main: " << ex.what() << endl;
        return -1;
    }

    return 0;
}
