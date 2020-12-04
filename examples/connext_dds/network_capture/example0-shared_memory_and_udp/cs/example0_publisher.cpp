/*
 */

#ifndef IMPORT_example0
#include "example0Support.h"
#endif

using namespace System;


public ref class example0Publisher {
public:
    static void publish(int domain_id, int sample_count);

private:
    static void shutdown(
        DDS::DomainParticipant^ participant);
};

int main(array<System::String^>^ argv) {
    int domain_id = 0;
    if (argv->Length >= 1) {
        domain_id = Int32::Parse(argv[0]);
    }

    int sample_count = 0; /* infinite loop */
    if (argv->Length >= 2) {
        sample_count = Int32::Parse(argv[1]);
    }

    try {
        example0Publisher::publish(
            domain_id, sample_count);
    }
    catch(DDS::Exception^) {
        return -1;
    }
    return 0;
}

void example0Publisher::publish(int domain_id, int sample_count) {

	/*
	* Enable network capture.
	*
	* This must be called before:
	*   - Any other network capture function is called.
	*   - Creating the participants for which we want to capture traffic.
	*/
	if (!NDDS::NetworkCapture::enable()) {
		throw gcnew ApplicationException("Error enabling network capture");
	}

	/* Start capturing traffic for all participants. */
	String^ filename = gcnew String("publisher");

	NDDS::NetworkCaptureParams_t^ params = gcnew NDDS::NetworkCaptureParams_t();
	params->traffic = NDDS::NetworkCaptureTrafficKind::TRAFFIC_OUT;
	params->transports->ensure_length(2, 128);
	params->transports->set_at(0, "shmem");
	params->transports->set_at(1, "udpv4");

	if (!NDDS::NetworkCapture::start(filename, params)) {
		throw gcnew ApplicationException("Error starting network capture");
	}

    DDS::DomainParticipant^ participant =
        DDS::DomainParticipantFactory::get_instance()->create_participant(
            domain_id,
            DDS::DomainParticipantFactory::PARTICIPANT_QOS_DEFAULT, 
            nullptr /* listener */,
            DDS::StatusMask::STATUS_MASK_NONE);
    if (participant == nullptr) {
        shutdown(participant);
        throw gcnew ApplicationException("create_participant error");
    }

    DDS::Publisher^ publisher = participant->create_publisher(
        DDS::DomainParticipant::PUBLISHER_QOS_DEFAULT,
        nullptr /* listener */,
        DDS::StatusMask::STATUS_MASK_NONE);
    if (publisher == nullptr) {
        shutdown(participant);
        throw gcnew ApplicationException("create_publisher error");
    }

    System::String^ type_name = example0TypeSupport::get_type_name();
    try {
        example0TypeSupport::register_type(participant, type_name);
    } catch (DDS::Exception^ e) {
        shutdown(participant);
        throw e;
    }

    DDS::Topic^ topic = participant->create_topic(
            "example0 using network capture C# API",
			type_name,
			DDS::DomainParticipant::TOPIC_QOS_DEFAULT,
			nullptr /* listener */,
			DDS::StatusMask::STATUS_MASK_NONE);
    if (topic == nullptr) {
        shutdown(participant);
        throw gcnew ApplicationException("create_topic error");
    }

    DDS::DataWriter^ writer = publisher->create_datawriter(
			topic,
			DDS::Publisher::DATAWRITER_QOS_DEFAULT,
			nullptr, /* listener */
			DDS::StatusMask::STATUS_MASK_NONE);
    if (writer == nullptr) {
        shutdown(participant);
        throw gcnew ApplicationException("create_datawriter error");
    }
    example0DataWriter^ example0_writer =
        safe_cast<example0DataWriter^>(writer);

    example0^ instance = example0TypeSupport::create_data();
    if (instance == nullptr) {
        shutdown(participant);
        throw gcnew ApplicationException(
            "example0TypeSupport::create_data error");
    }
    DDS::InstanceHandle_t instance_handle = DDS::InstanceHandle_t::HANDLE_NIL;

    const System::Int32 send_period = 4000; // milliseconds
    for (int count=0; (sample_count == 0) || (count < sample_count); ++count) {
        Console::WriteLine("Writing example0, count {0}", count);

        instance->msg = "Hello World! (" + count + ")";

        try {
            example0_writer->write(instance, instance_handle);
        }
        catch(DDS::Exception ^e) {
            Console::WriteLine("write error: {0}", e);
        }

		/*
		* Here we are going to pause capturing for some samples.
		* The resulting pcap file will not contain them.
		*/
		if (count == 5) {
			if (!NDDS::NetworkCapture::pause()) {
				throw gcnew ApplicationException("Error pausing network capture");
			}
		}
		else if (count == 10) {
			if (!NDDS::NetworkCapture::resume()) {
				throw gcnew ApplicationException("Error resuming network capture");
			}
		}

        System::Threading::Thread::Sleep(send_period);
    }

    try {
        example0TypeSupport::delete_data(instance);
    }
    catch(DDS::Exception ^e) {
        Console::WriteLine("example0TypeSupport::delete_data error: {0}", e);
    }

	/*
	* Before deleting the participants that are capturing, we must stop
	* network capture for them.
	*/
	if (!NDDS::NetworkCapture::stop()) {
		shutdown(participant);
		throw gcnew ApplicationException("Error stopping network capture");
	}
    shutdown(participant);
}

void example0Publisher::shutdown(
        DDS::DomainParticipant^ participant) {

    if (participant != nullptr) {
        participant->delete_contained_entities();
        DDS::DomainParticipantFactory::get_instance()->delete_participant(participant);
    }

	/*
	* Disable network capture.
	*
	* This must be:
	*   - The last network capture function that is called.
	*/
	if (!NDDS::NetworkCapture::disable()) {
		throw gcnew ApplicationException("Error disabling network capture");
	}

    DDS::DomainParticipantFactory::finalize_instance();
}
