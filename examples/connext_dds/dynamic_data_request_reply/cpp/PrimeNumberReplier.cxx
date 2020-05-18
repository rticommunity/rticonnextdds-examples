
#include <math.h>
#include <iostream>
#include <vector>
#include "PrimesType.cxx"




/*
 * Include the request-reply API
 */
#include "ndds/ndds_requestreply_cpp.h"

class PrimeNumberReplierExample {

private:
    DDS::DomainParticipant *participant;

public:

    PrimeNumberReplierExample(int domain_id)
    {
        /* Create the participant */
        participant = DDS::DomainParticipantFactory::get_instance()->
                create_participant(
                        domain_id,
                        DDS::PARTICIPANT_QOS_DEFAULT,
                        NULL /* listener */,
                        DDS::STATUS_MASK_NONE);

        if (participant == NULL) {
            throw std::runtime_error("create_participant error");
        }
    }

    ~PrimeNumberReplierExample()
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

private:

    void calculate_and_send_primes(
        connext::Replier<DDS_DynamicData, DDS_DynamicData>& replier,
        const DDS::SampleIdentity_t& request_sample_identity,
        DDS_Long n,
        DDS_Long primes_per_reply,
        DDS_DynamicData *reply)
        {

        /* Set Status to REPLY_IN_PROGRESS */
        reply->set_long("status", DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,0);

        /* Set the sequence: */
        DDS_LongSeq primes_seq;
        primes_seq.maximum(primes_per_reply);

        /* prime[i] indicates if i is a prime number
         * Initially, we assume all of them are prime
         */
        std::vector<bool> prime(n+1, true);

        prime[0] = false;
        prime[1] = false;

        int m = (int) sqrt((float)n);

        for (int i = 2; i <= m; i++) {
            if (prime[i]) {
                for (int k = i*i; k <= n; k+=i) {
                    prime[k] = false;
                }

                /* Add a new element */
                DDS::Long length = primes_seq.length();
                primes_seq.length(length + 1);
                primes_seq[length] = i;

                if (length + 1 == primes_per_reply) {
                    /* Send a reply now */
                    reply->set_long_seq(
                            "primes",
                            DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, 
                            primes_seq);

                    replier.send_reply(*reply, request_sample_identity);
                    primes_seq.length(0);
                }
            }
        }

        /* Calculation is done. Send remaining prime numbers */
        for (int i = m + 1; i <= n; i++) {
            if (prime[i]) {

                DDS::Long length = primes_seq.length();
                primes_seq.length(length + 1);
                primes_seq[length] = i;

                if (length + 1 == primes_per_reply) {
                    /* Send a reply now */
                    reply->set_long_seq("primes",
                        DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, 
                        primes_seq);
                    replier.send_reply(*reply, request_sample_identity);
                    primes_seq.length(0);
                }
            }
        }

        /* Send the last reply. Indicate that the calculation is complete and
         * send any prime number left in the sequence
         */
        reply->set_long("status", DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,1);
        /* Send a reply now */
        reply->set_long_seq(
                "primes",
                DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, 
                primes_seq);

        replier.send_reply(*reply, request_sample_identity);
    }

public:
    void run_example()
    {   

        DDS_TypeCodeFactory *factory = NULL;

        factory = DDS_TypeCodeFactory::get_instance();
        if (factory == NULL) {
            throw std::runtime_error(
                    "! Unable to get type code factory singleton");
        }


        // ----------- Create the request and reply typecode -----------------

        /* Create TypeCode for dynamic data type request
        */
        DDS_TypeCode *request_type  = get_prime_number_request_typecode(factory);
        if (request_type == NULL) {
            throw std::runtime_error("! Unable to create dynamic type code");
        }

        /* Create TypeCode for dynamic data type reply
        */
        DDS_TypeCode *reply_type  = type_w_PrimeNumberReply_typecode(factory);
        if (reply_type == NULL) {
            throw std::runtime_error("! Unable to create dynamic type code");
        }


        // ----------- Create the request and reply TypeSupport -----------------

        /* Create the Dynamic data type support object for request
        */
        DDS_DynamicDataTypeProperty_t props;
        DDSDynamicDataTypeSupport *request_type_support = 
                new DDSDynamicDataTypeSupport(
                        request_type, 
                        props);

        if (request_type_support == NULL) {
            throw std::runtime_error(
                    "! Unable to create dynamic data type support");

        }

        /* Create the Dynamic data type support object for reply
        */

        DDSDynamicDataTypeSupport *reply_type_support = 
                new DDSDynamicDataTypeSupport(reply_type, props);

        if (reply_type_support == NULL) {
            throw std::runtime_error(
                    "! Unable to create dynamic data type support");
        }

        /* Create parameters for replier */
        connext::ReplierParams<DDS_DynamicData, DDS_DynamicData>
                replier_params(participant);


        replier_params.service_name("PrimeCalculator");
        replier_params.qos_profile(
                "RequestReplyExampleProfiles",
                "ReplierExampleProfile");
                
        replier_params.request_type_support(request_type_support);
        replier_params.reply_type_support(reply_type_support);

        DDS_DynamicData *reply = reply_type_support->create_data();
         
        /* In this example we create the replier on the stack, but you
         * can create on the heap as well
         */
        connext::Replier<DDS_DynamicData, DDS_DynamicData> replier(
                replier_params);

        /*
         * Receive requests and process them
         */

        const DDS::Duration_t MAX_WAIT = {20, 0};

        /* receive_request returns false when it times out */
        connext::LoanedSamples<DDS_DynamicData>received_requests = 
                replier.receive_requests(MAX_WAIT);

        bool stop=1;

        while(stop) {

            typedef connext::LoanedSamples<DDS_DynamicData>::iterator iterator;
            for (
                    iterator request_sample = received_requests.begin(); 
                    request_sample != received_requests.end(); 
                    ++request_sample) 
            {
                
                if(!request_sample->info().valid_data) {
                    continue;
                }
                
                /* We obtain "n" and "primes_per_reply" 
                 * from the received sample:
                 */
                
                DDS_Long n, primes_per_reply; 

                request_sample->data().get_long(
                        n,
                        "n",
                        DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);

                request_sample->data().get_long(
                        primes_per_reply,
                        "primes_per_reply",
                        DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);

                /* This constant is defined in PrimesType.cxx */
                if (n <= 0 || primes_per_reply <= 0 
                        || primes_per_reply > THE_PRIME_SEQUENCE_MAX_LENGTH)
                {

                    std::cout << "Cannot process request" << std::endl;

                    /* Send reply indicating error */
                    reply->set_long(
                            "status",
                            DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,
                            2);

                    replier.send_reply(
                            *reply, 
                            request_sample->identity());
                }
                else{
                    std::cout << "Calculating prime numbers below "
                          << n << "... " << std::endl;
                }

                calculate_and_send_primes(
                        replier,
                        request_sample->identity(),
                        n,
                        primes_per_reply,
                        reply);

                std::cout << "DONE" << std::endl;
 
            }

            stop=0;
        }
    }
};

int replier_main(int domain_id) {
    /* Uncomment this to turn on additional logging
        NDDSConfigLogger::get_instance()->set_verbosity(
            NDDS_CONFIG_LOG_VERBOSITY_WARNING);
    */
    std::cout << "PrimeNumberReplier running (on domain " << domain_id << ")"
              << std::endl;

    try {
        /*
         * Run the example
         */
        PrimeNumberReplierExample(domain_id).run_example();

    } catch (const std::exception& ex) {
        std::cout << "Exception: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}

#if !(defined(RTI_VXWORKS) && !defined(__RTP__)) && !defined(RTI_PSOS)
int main(int argc, char *argv[])
{
    int domain_id = 0;

    if (argc > 1) {
        domain_id = atoi(argv[1]);
    }
    return replier_main(domain_id);

}
#endif


