/*****************************************************************************/
/*         (c) Copyright, Real-Time Innovations, All rights reserved.        */
/*                                                                           */
/*         Permission to modify and use for internal purposes granted.       */
/* This software is provided "as is", without warranty, express or implied.  */
/*                                                                           */
/*****************************************************************************/

#include <algorithm>
#include <iostream>
#include <dds/sub/ddssub.hpp>
#include <dds/core/ddscore.hpp>

using namespace dds::core;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::sub;

// Domain id
const int DOMAIN_ID = 0;

// The listener of events and data from the middleware
class HelloReaderListener : public NoOpDataReaderListener<StringTopicType> {
public:
	HelloReaderListener() : must_shutdown(false) { }

	bool get_must_shutdown() {
		return must_shutdown;
	}

	void on_data_available(DataReader<StringTopicType>& reader) {
		// Take all samples
		LoanedSamples<StringTopicType> samples = reader.take();

		for (LoanedSamples<StringTopicType>::iterator sample_it = samples.begin();
			sample_it != samples.end(); sample_it++) {

			// Valid (this isn't just a lifecycle sample): print it
			if (sample_it->info().valid()) {
				const dds::core::string& sample = sample_it->data();
				cout << sample << endl;

				// Empty sample to quit
				if (sample.size() == 0)
					must_shutdown = true;
			}
		}
	}

private:
	bool must_shutdown;
};

int main() {
	// --- Set Up --------------------------------------------------------- //

	// To turn on additional logging, include <rti/config/Logger.hpp> and
    // uncomment the following line:
    // rti::config::Logger::instance().verbosity(rti::config::Verbosity::STATUS_ALL);

	// Create a DomainParticipant with default QoS.
	DomainParticipant participant (DOMAIN_ID);

	// Create a Topic and automatically register the type.
	Topic<StringTopicType> topic (participant, "HelloSimple");

	// Create a DataReader with default QoS.
	// The subscriber is created in-line.
	DataReader<StringTopicType> reader (Subscriber(participant), topic);

	// Set the listener to the subscriber.
	HelloReaderListener listener;
	reader.listener(&listener, dds::core::status::StatusMask::data_available());

	// --- Sleep During Asynchronous Reception ---------------------------- //

    // This thread sleeps forever. When a sample is received, RTI Data
    // Distribution Service will call the on_data_available_callback function.
	std::cout << "Ready to read data." << std::endl;
	std::cout << "Press CTRL+C to terminate." << std::endl;
	while (!listener.get_must_shutdown()) {
		rti::util::sleep(Duration(4));
	}

	std::cout << "Shutting down" << std::endl;
	return 0;
}
