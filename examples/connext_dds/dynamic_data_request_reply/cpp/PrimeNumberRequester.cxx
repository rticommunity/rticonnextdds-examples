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

#include <iostream>
#include "PrimesType.cxx"
#include "ndds/ndds_requestreply_cpp.h" /* Include the request-reply API */

class PrimeNumberRequesterExample {

private:
    DDS::DomainParticipant * participant;

    void fill_data(
            DDS_DynamicData *sample,
            DDS_TypeCodeFactory *factory,
            int n, int primes_per_reply) {

        DDS_ReturnCode_t rc;

        rc = sample->set_long(
                "n", 
                DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, 
                n);

        if (rc != DDS_RETCODE_OK) {
            std::cerr << "! Unable to set value (" << n
                      << ") for sampleId. Error=" << rc << std::endl;
        }

        rc = sample->set_long("primes_per_reply", 
                        DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, 
                        primes_per_reply);

        if (rc != DDS_RETCODE_OK) {
            std::cerr << "! Unable to set value (" << primes_per_reply
                      << ") for sampleId. Error=" << rc << std::endl;
        }
    }

public:
    PrimeNumberRequesterExample(int domain_id)
    {
        /* Create the participant */
        participant = DDS::DomainParticipantFactory::get_instance()->
            create_participant(
                    domain_id,
                    DDS::PARTICIPANT_QOS_DEFAULT,
                    NULL /* listener */,
                    DDS::STATUS_MASK_NONE);

        if (participant == NULL) {
            throw std::runtime_error("! create_participant error");
        }
    }

    ~PrimeNumberRequesterExample()
    {
        /* Delete the participant and all contained entities */

        DDS::ReturnCode_t retcode = participant->delete_contained_entities();
        if (retcode != DDS::RETCODE_OK) {
            fprintf(stderr, "delete_contained_entities error %d\n", retcode);
        }

        retcode = DDS::DomainParticipantFactory::get_instance()->
            delete_participant(participant);
        if (retcode != DDS::RETCODE_OK) {
            fprintf(stderr, "delete_participant error %d\n", retcode);
        }
    }

    void run_example(int n, int primes_per_reply)
    {
        DDS_TypeCodeFactory *factory = NULL;

        factory = DDS_TypeCodeFactory::get_instance();
        if (factory == NULL) {
            throw std::runtime_error(
                    "! Unable to get type code factory singleton");
        }


        /* ----------- Create the request and reply typecode ---------------- */

        /* Create TypeCode for dynamic data type request
        */
        DDS_TypeCode * request_type  = get_prime_number_request_typecode(factory);
        if (request_type == NULL) {
            throw std::runtime_error(
                    "! Unable to create dynamic request type code");
        }

        /* Create TypeCode for dynamic data type reply */
        DDS_TypeCode * reply_type  = type_w_PrimeNumberReply_typecode(factory);
        if (reply_type == NULL) {
            throw std::runtime_error(
                    "! Unable to create dynamic reply type code");
        }


        /* ----------- Create the request and reply TypeSupport ------------- */

        /* Create the Dynamic data type support object for request */
        DDS_DynamicDataTypeProperty_t props;
        DDSDynamicDataTypeSupport *request_type_support =
                new DDSDynamicDataTypeSupport(request_type, props);

        if (request_type_support == NULL) {
            throw std::runtime_error(
                    "! Unable to create dynamic data type support for request");
        }

        /* Create the Dynamic data type support object for request */
        DDSDynamicDataTypeSupport *reply_type_support = 
                new DDSDynamicDataTypeSupport(reply_type, props);

        if (reply_type_support == NULL) {
            throw std::runtime_error(
                    "! Unable to create dynamic data type support for reply");
        }
 

        DDS_DynamicData *sample = request_type_support->create_data();

        fill_data(sample, factory,  n,  primes_per_reply);

        /* 
         * Create the requester with the participant, and a QoS profile
         * defined in USER_QOS_PROFILES.xml
         */
        connext::RequesterParams requester_params(participant);
        requester_params.service_name("PrimeCalculator");
        requester_params.qos_profile(
                "RequestReplyExampleProfiles",
                "RequesterExampleProfile");
        requester_params.request_type_support(request_type_support);
        requester_params.reply_type_support(reply_type_support);

        /* Create the requester: */
        connext::Requester<DDS_DynamicData, DDS_DynamicData> requester(
            requester_params);


        /* Send the request */
        requester.send_request(*sample);

        /* Receive replies */
        const DDS::Duration_t MAX_WAIT = {20, 0};

        bool in_progress = true;
        while(in_progress) {
            connext::LoanedSamples<DDS_DynamicData> replies =
                requester.receive_replies(MAX_WAIT);

            /* 
             * When receive_replies times out,
             * it returns an empty reply collection
             */
            if (replies.length() == 0) {
                throw std::runtime_error("! Timed out waiting for replies");
                return;
            }

            /* Print the prime numbers we receive */
            typedef connext::LoanedSamples<DDS_DynamicData>::iterator iterator;
            for (iterator it = replies.begin(); it != replies.end(); ++it) {
                if (it->info().valid_data) {

                    
                    DDS_PrintFormatProperty proper;

                    /* Get the status: */
                    DDS_Long status;
                    it->data().get_long(status, "status",
                    DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);

                    /* Get the sequence of primes: */
                    DDS_LongSeq primes_seq;
                    primes_seq.ensure_length(
                            primes_per_reply,
                            THE_PRIME_SEQUENCE_MAX_LENGTH);

                    it->data().get_long_seq(primes_seq,
                        "primes", 
                        DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);
                    
                    /* We print the primes: */
                    for (int i = 0; i < primes_seq.length(); i++) {
                        std::cout << primes_seq[i] << " ";
                    }

                    if (status != 0) { /* REPLY_IN_PROGRESS */
                        in_progress = false;
                        if (status == 2) { /* REPLY_ERROR */
                            throw std::runtime_error("! Error in replier");
                        } else { /* reply->status == COMPLETED */
                            std::cout << "DONE" << std::endl;
                        }
                    }
  
                }
            }

            /* 
             * We don't need to call replies.return_loan(); the destructor
             * takes care of doing it every time replies go out of scope
             */
        }
    }

};

int requester_main(int n, int primes_per_reply, int domain_id)
{
    /* Uncomment this to turn on additional logging
        NDDSConfigLogger::get_instance()->set_verbosity(
            NDDS_CONFIG_LOG_VERBOSITY_WARNING);
    */
    std::cout << "PrimeNumberRequester: Sending a request to calculate the "
              << "prime numbers <= " << n << " in sequences of "
              << primes_per_reply << " or less elements (on domain "
              << domain_id << ")" << std::endl;

    try {
        /*
         * Run the example
         */
        PrimeNumberRequesterExample(domain_id).run_example(n, primes_per_reply);

    } catch (const std::exception& ex) {
        std::cout << "Exception: " << ex.what() << std::endl;
        return(-1);
    }

    return 0;
}

int main(int argc, char *argv[])
{

    int domain_id = 0; /* Default Domain ID if not specified */
    int n;
    int primes_per_reply = 5;

    if (argc < 2) {
        std::cout << "PrimeNumberRequester" << std::endl
                  << "Sends a request to calculate the prime numbers <= n"
                  << std::endl
                  << "Parameters: <n> [<primes_per_reply>=5] [<domain_id>=0]"
                  << std::endl;
        return(-1);
    }

    n = atoi(argv[1]);

    if (argc > 2) {
        primes_per_reply = atoi(argv[2]);
    }

    if (argc > 3) {
        domain_id = atoi(argv[3]);
    }

    return requester_main(n, primes_per_reply, domain_id);
}
#endif
