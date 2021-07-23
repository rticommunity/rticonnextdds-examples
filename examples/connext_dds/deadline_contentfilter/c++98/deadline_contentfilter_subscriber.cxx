/*******************************************************************************
 (c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/

/* This example sets the deadline period to 2 seconds to trigger a deadline if
   the DataWriter does not update often enough, or if the content-filter
   filters out data so there is no data available within 2 seconds.

   This example starts by filtering out instances >= 2, and changes to later
   filter out instances >= 1.

*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>

#include "deadline_contentfilter.h"
#include "deadline_contentfilterSupport.h"
#include "ndds/ndds_cpp.h"
#include "application.h"

using namespace application;

//// Changes for Deadline
// For timekeeping
#include <time.h>
clock_t init;

static int shutdown_participant(
        DDSDomainParticipant *participant,
        const char *shutdown_message,
        int status);

class deadline_contentfilterListener : public DDSDataReaderListener {
public:
    virtual void on_requested_deadline_missed(
            DDSDataReader * /*reader*/,
            const DDS_RequestedDeadlineMissedStatus & /*status*/);

    virtual void on_requested_incompatible_qos(
            DDSDataReader * /*reader*/,
            const DDS_RequestedIncompatibleQosStatus & /*status*/)
    {
    }

    virtual void on_sample_rejected(
            DDSDataReader * /*reader*/,
            const DDS_SampleRejectedStatus & /*status*/)
    {
    }

    virtual void on_liveliness_changed(
            DDSDataReader * /*reader*/,
            const DDS_LivelinessChangedStatus & /*status*/)
    {
    }

    virtual void on_sample_lost(
            DDSDataReader * /*reader*/,
            const DDS_SampleLostStatus & /*status*/)
    {
    }

    virtual void on_subscription_matched(
            DDSDataReader * /*reader*/,
            const DDS_SubscriptionMatchedStatus & /*status*/)
    {
    }

    virtual void on_data_available(DDSDataReader *reader);
};

/* Start changes for Deadline */
void deadline_contentfilterListener::on_requested_deadline_missed(
        DDSDataReader *reader,
        const DDS_RequestedDeadlineMissedStatus &status)
{
    double elapsed_ticks = clock() - init;
    double elapsed_secs = elapsed_ticks / CLOCKS_PER_SEC;

    /* Creates a temporary object of our structure "deadline_contentfilter"
     * in order to find out which instance missed its deadline period.  The
     * get_key_value call only fills in the values of the key fields inside
     * the dummy object.
     */
    deadline_contentfilter dummy;
    DDS_ReturnCode_t retcode =
            ((deadline_contentfilterDataReader *) reader)
                    ->get_key_value(dummy, status.last_instance_handle);

    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "get_key_value error " << retcode << std::endl;
        return;
    }

    std::cout << std::fixed;
    std::cout << std::setprecision(2);

    // Print out which instance missed its deadline.
    std::cout << "Missed deadline @ t=" << elapsed_secs
              << " on instance code = " << dummy.code << std::endl;
}
/* End changes for Deadline */

void deadline_contentfilterListener::on_data_available(DDSDataReader *reader)
{
    deadline_contentfilterDataReader *typed_reader = NULL;
    deadline_contentfilterSeq data_seq;
    DDS_SampleInfoSeq info_seq;
    DDS_ReturnCode_t retcode;

    typed_reader = deadline_contentfilterDataReader::narrow(reader);
    if (typed_reader == NULL) {
        std::cerr << "DataReader narrow error\n";
        return;
    }

    retcode = typed_reader->take(
            data_seq,
            info_seq,
            DDS_LENGTH_UNLIMITED,
            DDS_ANY_SAMPLE_STATE,
            DDS_ANY_VIEW_STATE,
            DDS_ANY_INSTANCE_STATE);

    if (retcode == DDS_RETCODE_NO_DATA) {
        return;
    } else if (retcode != DDS_RETCODE_OK) {
        std::cerr << "take error " << retcode << std::endl;
        return;
    }

    std::cout << std::fixed;
    std::cout << std::setprecision(2);

    for (int i = 0; i < data_seq.length(); ++i) {
        if (info_seq[i].valid_data) {
            /* Start changes for Deadline */
            /* print the time we get each sample. */
            double elapsed_ticks = clock() - init;
            double elapsed_secs = elapsed_ticks / CLOCKS_PER_SEC;

            std::cout << "@ t=" << elapsed_secs << ", Instance"
                      << data_seq[i].code << ": <" << data_seq[i].x << ","
                      << data_seq[i].y << ">\n";
            /* deadlineTypeSupport::print_data(&data_seq[i]); */
            /* End changes for Deadline */
        }
    }

    retcode = typed_reader->return_loan(data_seq, info_seq);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "return loan error " << retcode << std::endl;
    }
}

int run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    struct DDS_Duration_t receive_period = { 1, 0 };

    /* Changes for Deadline */
    /* for timekeeping */
    init = clock();

    // Start communicating in a domain, usually one participant per application
    DDSDomainParticipant *participant =
            DDSTheParticipantFactory->create_participant(
                    domain_id,
                    DDS_PARTICIPANT_QOS_DEFAULT,
                    NULL /* listener */,
                    DDS_STATUS_MASK_NONE);
    if (participant == NULL) {
        return shutdown_participant(
                participant,
                "create_participant error",
                EXIT_FAILURE);
    }

    // A Subscriber allows an application to create one or more DataReaders
    DDSSubscriber *subscriber = participant->create_subscriber(
            DDS_SUBSCRIBER_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (subscriber == NULL) {
        return shutdown_participant(
                participant,
                "create_subscriber error",
                EXIT_FAILURE);
    }

    // Register the datatype to use when creating the Topic
    const char *type_name = deadline_contentfilterTypeSupport::get_type_name();
    DDS_ReturnCode_t retcode = deadline_contentfilterTypeSupport::register_type(
            participant,
            type_name);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "register_type error",
                EXIT_FAILURE);
    }

    // Create a Topic with a name and a datatype
    DDSTopic *topic = participant->create_topic(
            "Example deadline_contentfilter",
            type_name,
            DDS_TOPIC_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (topic == NULL) {
        return shutdown_participant(
                participant,
                "create_topic error",
                EXIT_FAILURE);
    }

    /* Start changes for Deadline */
    /* Set up content filtered topic to show interaction with deadline */
    DDS_StringSeq parameters(1);
    const char *param_list[] = { "2" };
    parameters.from_array(param_list, 1);

    DDSContentFilteredTopic *cft = NULL;
    cft = participant->create_contentfilteredtopic(
            "ContentFilteredTopic",
            topic,
            "code < %0",
            parameters);

    /* Get default datareader QoS to customize */
    DDS_DataReaderQos datareader_qos;
    retcode = subscriber->get_default_datareader_qos(datareader_qos);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "get_default_datareader_qos error",
                EXIT_FAILURE);
    }

    /* Create data reader listener */
    deadline_contentfilterListener *reader_listener =
            new deadline_contentfilterListener();
    if (reader_listener == NULL) {
        return shutdown_participant(
                participant,
                "listener instantiation error",
                EXIT_FAILURE);
    }

    // This DataReader reads data on "Example deadline_contentfilter" Topic
    DDSDataReader *untyped_reader = subscriber->create_datareader(
            cft,
            DDS_DATAREADER_QOS_DEFAULT,
            reader_listener,
            DDS_STATUS_MASK_ALL);
    if (untyped_reader == NULL) {
        return shutdown_participant(
                participant,
                "create_datareader error",
                EXIT_FAILURE);
    }
    /* If you want to change the DataReader's QoS programmatically rather than
     * using the XML file, you will need to add the following lines to your
     * code and comment out the create_datareader call above.
     *
     * In this case, we set the deadline period to 2 seconds to trigger
     * a deadline if the DataWriter does not update often enough, or if
     * the content-filter filters out data so there is no data available
     * with 2 seconds.
     */


    /* Set deadline QoS */
    /*    DDS_Duration_t deadline_period = {2, 0};
        datareader_qos.deadline.period = deadline_period;

        DDSDataReader *untyped_reader = subscriber->create_datareader(
            cft, datareader_qos, reader_listener,
            DDS_STATUS_MASK_ALL);
        if (untyped_reader == NULL) {
            return shutdown_participant(
                participant,
                "create_datareader error",
                EXIT_FAILURE);
        }
    */

    /* Main loop */
    for (int count = 0; count < sample_count && !shutdown_requested; ++count) {
        /* After 10 seconds, change content filter to accept only instance 0 */
        if (count == 10) {
            std::cout << "Starting to filter out instance1\n";
            parameters[0] = DDS_String_dup("1");
            cft->set_expression_parameters(parameters);
        }
        NDDSUtility::sleep(receive_period);
    }

    delete reader_listener;

    /* End changes for Deadline */

    // Cleanup
    return shutdown_participant(participant, "Shutting down", 0);
}

// Delete all entities
static int shutdown_participant(
        DDSDomainParticipant *participant,
        const char *shutdown_message,
        int status)
{
    DDS_ReturnCode_t retcode;

    std::cout << shutdown_message << std::endl;

    if (participant != NULL) {
        // Cleanup everything created by this Participant
        retcode = participant->delete_contained_entities();
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "delete_contained_entities error" << retcode
                      << std::endl;
            status = EXIT_FAILURE;
        }

        retcode = DDSTheParticipantFactory->delete_participant(participant);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "delete_participant error" << retcode << std::endl;
            status = EXIT_FAILURE;
        }
    }
    return status;
}

int main(int argc, char *argv[])
{
    // Parse arguments and handle control-C
    ApplicationArguments arguments;
    parse_arguments(arguments, argc, argv);
    if (arguments.parse_result == PARSE_RETURN_EXIT) {
        return EXIT_SUCCESS;
    } else if (arguments.parse_result == PARSE_RETURN_FAILURE) {
        return EXIT_FAILURE;
    }
    setup_signal_handlers();

    // Sets Connext verbosity to help debugging
    NDDSConfigLogger::get_instance()->set_verbosity(arguments.verbosity);

    int status = run_subscriber_application(
            arguments.domain_id,
            arguments.sample_count);

    // Releases the memory used by the participant factory.  Optional at
    // application exit
    DDS_ReturnCode_t retcode = DDSDomainParticipantFactory::finalize_instance();
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "finalize_instance error" << retcode << std::endl;
        status = EXIT_FAILURE;
    }

    return status;
}
