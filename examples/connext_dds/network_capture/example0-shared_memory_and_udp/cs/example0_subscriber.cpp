/*
* (c) Copyright, Real-Time Innovations, 2021.  All rights reserved.
* RTI grants Licensee a license to use, modify, compile, and create derivative
* works of the software solely for use with RTI Connext DDS. Licensee may
* redistribute copies of the software provided that all such copies are subject
* to this license. The software is provided "as is", with no warranty of any
* type, including any warranty for fitness for any purpose. RTI is under no
* obligation to maintain or support the software. RTI shall not be liable for
* any incidental or consequential damages arising out of the use or inability
* to use the software.
*/

#ifndef IMPORT_example0
#include "example0Support.h"
#endif

using namespace System;

public ref class example0Subscriber {
  public:
    static void subscribe(int domain_id, int sample_count);

  private:
    static void shutdown(
        DDS::DomainParticipant^ participant);
};

public ref class example0Listener : public DDS::DataReaderListener {
  public:
    virtual void on_requested_deadline_missed(
        DDS::DataReader^ /*reader*/,
        DDS::RequestedDeadlineMissedStatus% /*status*/) override {}

    virtual void on_requested_incompatible_qos(
        DDS::DataReader^ /*reader*/,
        DDS::RequestedIncompatibleQosStatus^ /*status*/) override {}

    virtual void on_sample_rejected(
        DDS::DataReader^ /*reader*/,
        DDS::SampleRejectedStatus% /*status*/) override {}

    virtual void on_liveliness_changed(
        DDS::DataReader^ /*reader*/,
        DDS::LivelinessChangedStatus% /*status*/) override {}

    virtual void on_sample_lost(
        DDS::DataReader^ /*reader*/,
        DDS::SampleLostStatus% /*status*/) override {}

    virtual void on_subscription_matched(
        DDS::DataReader^ /*reader*/,
        DDS::SubscriptionMatchedStatus% /*status*/) override {}

    virtual void on_data_available(DDS::DataReader^ reader) override;

    example0Listener() {
        data_seq = gcnew example0Seq();
        info_seq = gcnew DDS::SampleInfoSeq();
    }

  private:
    example0Seq^ data_seq;
    DDS::SampleInfoSeq^ info_seq;
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
        example0Subscriber::subscribe(
            domain_id, sample_count);
    }
    catch(DDS::Exception^) {
        return -1;
    }
    return 0;
}

void example0Subscriber::subscribe(
		int domain_id, int sample_count)
{
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

	/* Start capturing traffic for the participant. */
	String^ filename = gcnew String("subscriber");
	if (!NDDS::NetworkCapture::start(participant, filename)) {
		throw gcnew ApplicationException("Error starting network capture");
	}

    DDS::Subscriber^ subscriber = participant->create_subscriber(
			DDS::DomainParticipant::SUBSCRIBER_QOS_DEFAULT,
			nullptr, /* listener */
			DDS::StatusMask::STATUS_MASK_NONE);
    if (subscriber == nullptr) {
        shutdown(participant);
        throw gcnew ApplicationException("create_subscriber error");
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
			nullptr, /* listener */
			DDS::StatusMask::STATUS_MASK_NONE);
    if (topic == nullptr) {
        shutdown(participant);
        throw gcnew ApplicationException("create_topic error");
    }

    example0Listener^ reader_listener = gcnew example0Listener();
    DDS::DataReader^ reader = subscriber->create_datareader(
        topic,
        DDS::Subscriber::DATAREADER_QOS_DEFAULT,
        reader_listener,
        DDS::StatusMask::STATUS_MASK_ALL);
    if (reader == nullptr) {
        shutdown(participant);
        throw gcnew ApplicationException("create_datareader error");
    }

    const System::Int32 receive_period = 4000; // milliseconds
    for (int count=0; (sample_count == 0) || (count < sample_count); ++count) {
        Console::WriteLine(
				"example0 subscriber sleeping for {0} sec...",
				receive_period / 1000);

		/*
		* Here we are going to pause capturing for some samples.
		* The resulting pcap file will not contain them.
		*/
		if (count == 5) {
			if (!NDDS::NetworkCapture::pause(participant)) {
				throw gcnew ApplicationException("Error pausing network capture");
			}
		}
		else if (count == 10) {
			if (!NDDS::NetworkCapture::resume(participant)) {
				throw gcnew ApplicationException("Error resuming network capture");
			}
		}

        System::Threading::Thread::Sleep(receive_period);
    }

	/*
	* Before deleting the participants that are capturing, we must stop
	* network capture for them.
	*/
	if (!NDDS::NetworkCapture::stop(participant)) {
		shutdown(participant);
		throw gcnew ApplicationException("Error stopping network capture");
	}
    shutdown(participant);
}

void example0Subscriber::shutdown(
    DDS::DomainParticipant^ participant) {

    if (participant != nullptr) {
        participant->delete_contained_entities();
        DDS::DomainParticipantFactory::get_instance()->delete_participant(
            participant);
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

void example0Listener::on_data_available(DDS::DataReader^ reader) {
    example0DataReader^ example0_reader =
    safe_cast<example0DataReader^>(reader);

    try {
        example0_reader->take(
            data_seq,
            info_seq,
            DDS::ResourceLimitsQosPolicy::LENGTH_UNLIMITED,
            DDS::SampleStateKind::ANY_SAMPLE_STATE,
            DDS::ViewStateKind::ANY_VIEW_STATE,
            DDS::InstanceStateKind::ANY_INSTANCE_STATE);
    }
    catch(DDS::Retcode_NoData^) {
        return;
    }
    catch(DDS::Exception ^e) {
        Console::WriteLine("take error {0}", e);
        return;
    }

    System::Int32 data_length = data_seq->length;
    for (int i = 0; i < data_length; ++i) {
        if (info_seq->get_at(i)->valid_data) {
            example0TypeSupport::print_data(data_seq->get_at(i));
        }
    }

    try {
        example0_reader->return_loan(data_seq, info_seq);
    }
    catch(DDS::Exception ^e) {
        Console::WriteLine("return loan error {0}", e);
    }
}
