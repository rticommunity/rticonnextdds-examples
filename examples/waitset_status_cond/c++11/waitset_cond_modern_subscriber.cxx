/*******************************************************************************
 (c) 2005-2017 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/

#include <algorithm>
#include <iostream>

#include <dds/sub/ddssub.hpp>
#include <dds/core/ddscore.hpp>
// Or simply include <dds/dds.hpp>

#include "waitset_cond_modern.hpp"

int subscriber_main(int domain_id, int sample_count)
{
    // Create a DomainParticipant with default Qos
    dds::domain::DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type
    dds::topic::Topic<Foo> topic(participant, "Example Foo");

    // Create a DataReader with default Qos (Subscriber created in-line)
    dds::sub::DataReader<Foo> reader(dds::sub::Subscriber(participant), topic);

    // Create a Status Condition for the reader
    dds::core::cond::StatusCondition status_condition(reader);
 
    //Enable statuses configuration for the status that it is desired
    status_condition.enabled_statuses(dds::core::status::StatusMask::liveliness_changed());

    // Lambda function for the status_condition
    // Handler register a custom handler with the condition
    status_condition->handler([&reader]() {
        // Get the status changes so we can check witch status condition triggered
        dds::core::status::StatusMask status_mask = reader.status_changes();
        // In Case of Liveliness changed
        if ((status_mask &
                dds::core::status::StatusMask::liveliness_changed()).any()) {
            dds::core::status::LivelinessChangedStatus st =
            reader.liveliness_changed_status();
            std::cout << "Liveliness changed => Active writers = "
                    << st.alive_count() << std::endl;
        }
    }
    ); // Create a ReadCondition for any data on this reader and associate a handler

    int count = 0;
    dds::sub::cond::ReadCondition read_condition(
            reader,
            dds::sub::status::DataState::any(),
            [&reader, &count]()
    {
        // Take all samples
        dds::sub::LoanedSamples<Foo> samples = reader.take();
        for (auto sample : samples){
            if (sample.info().valid()){
                count++;
                std::cout << sample.data() << std::endl;
            }
        }
    } // The LoanedSamples destructor returns the loan
    );

    // Create a WaitSet and attach both ReadCondition and StatusCondition
    dds::core::cond::WaitSet waitset;
    waitset += read_condition;
    waitset += status_condition;

    while (count < sample_count || sample_count == 0) {
        // Dispatch will call the handlers associated to the WaitSet conditions
        // when they activate
        waitset.dispatch(dds::core::Duration(4)); // Wait up to 4s each time
    }
    return 1;
}

int main(int argc, char *argv[])
{

    int domain_id = 0;
    int sample_count = 0; // infinite loop

    if (argc >= 2) {
        domain_id = atoi(argv[1]);
    }
    if (argc >= 3) {
        sample_count = atoi(argv[2]);
    }

    // To turn on additional logging, include <rti/config/Logger.hpp> and
    // uncomment the following line:
    // rti::config::Logger::instance().verbosity(rti::config::Verbosity::STATUS_ALL);

    try {
        subscriber_main(domain_id, sample_count);
    } catch (const std::exception& ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in subscriber_main(): " << ex.what() << std::endl;
        return -1;
    }

    // RTI Connext provides a finalize_participant_factory() method
    // if you want to release memory used by the participant factory singleton.
    // Uncomment the following line to release the singleton:
    //
    // dds::domain::DomainParticipant::finalize_participant_factory();

    return 0;
}
