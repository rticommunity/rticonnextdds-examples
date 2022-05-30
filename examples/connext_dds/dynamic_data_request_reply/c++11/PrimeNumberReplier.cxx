/*******************************************************************************
 (c) 2005-2022 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/

#include "Primes.hpp"
#include <rti/request/rtirequest.hpp>  // full request-reply API
#include <rti/config/Logger.hpp>       // Logger to configure logging verbosity

using namespace dds::core::xtypes;

class PrimeNumberReplierExample {
private:
    dds::domain::DomainParticipant participant;
    PrimeNumberTypeBuilder type_builder;

public:
    PrimeNumberReplierExample(int domain_id) : participant(domain_id)
    {
    }

private:
    void calculate_and_send_primes(
            rti::request::Replier<DynamicData, DynamicData> replier,
            const rti::sub::LoanedSample<DynamicData> &request)
    {
        DynamicData request_data(request.data());
        int n = request_data.value<int32_t>("n");
        size_t primes_per_reply = static_cast<size_t>(
                request_data.value<int32_t>("primes_per_reply"));

        DynamicData reply(type_builder.reply_type());
        reply.value<int32_t>(
                "status",
                static_cast<int32_t>(
                        PrimeNumberCalculationStatus::REPLY_IN_PROGRESS));

        // prime[i] indicates if i is a prime number
        // Initially, we assume all of them are prime
        std::vector<bool> prime(n + 1, true);

        prime[0] = false;
        prime[1] = false;

        int m = (int) std::sqrt((float) n);
        std::vector<int32_t> primes;

        for (int i = 2; i <= m; i++) {
            if (prime[i]) {
                for (int k = i * i; k <= n; k += i) {
                    prime[k] = false;
                }

                // Add a new element
                primes.push_back(i);

                if (primes.size() == primes_per_reply) {
                    // Send a reply now
                    reply.set_values("primes", primes);
                    replier.send_reply(reply, request.info());
                    primes.clear();
                }
            }
        }

        // Calculation is done. Send remaining prime numbers
        for (int i = m + 1; i <= n; i++) {
            if (prime[i]) {
                primes.push_back(i);

                if (primes.size() == primes_per_reply) {
                    // Send a reply now
                    reply.set_values("primes", primes);
                    replier.send_reply(reply, request.info());
                    primes.clear();
                }
            }
        }

        // Send the last reply. Indicate that the calculation is complete and
        // send any prime number left in the sequence
        reply.set_values("primes", primes);
        reply.value<int32_t>(
                "status",
                static_cast<int32_t>(
                        PrimeNumberCalculationStatus::REPLY_COMPLETED));
        replier.send_reply(reply, request.info());
    }

public:
    void run_example()
    {
        rti::request::ReplierParams replier_params(participant);

        auto qos_provider = dds::core::QosProvider::Default();
        replier_params.service_name("PrimeCalculator");
        replier_params.datawriter_qos(qos_provider.datawriter_qos(
                "RequestReplyExampleProfiles::ReplierExampleProfile"));
        replier_params.datareader_qos(qos_provider.datareader_qos(
                "RequestReplyExampleProfiles::ReplierExampleProfile"));

        replier_params.request_type(type_builder.request_type());
        replier_params.reply_type(type_builder.reply_type());

        rti::request::Replier<DynamicData, DynamicData> replier(replier_params);

        // Receive requests and process them
        const auto MAX_WAIT = dds::core::Duration::from_secs(20);
        auto requests = replier.receive_requests(MAX_WAIT);
        while (requests.length() > 0) {  // end the requester when no requests
                                         // received in MAX_WAIT
            for (const auto &request : requests) {
                if (!request.info().valid()) {
                    continue;
                }

                DynamicData request_data(request.data());
                int n = request_data.value<int32_t>("n");
                size_t primes_per_reply = static_cast<size_t>(
                        request_data.value<int32_t>("primes_per_reply"));

                // This constant is defined in Primes.h
                if (n <= 0 || primes_per_reply <= 0
                    || primes_per_reply > PRIME_SEQUENCE_MAX_LENGTH) {
                    std::cout << "Cannot process request" << std::endl;

                    // Send reply indicating error
                    DynamicData error_reply(type_builder.reply_type());
                    error_reply.value<int32_t>(
                            "status",
                            static_cast<int32_t>(
                                    PrimeNumberCalculationStatus::REPLY_ERROR));
                    replier.send_reply(error_reply, request.info());

                } else {
                    std::cout << "Calculating prime numbers below " << n
                              << "... " << std::endl;

                    calculate_and_send_primes(replier, request);

                    std::cout << "DONE" << std::endl;
                }
            }

            requests = replier.receive_requests(MAX_WAIT);
        }
    }
};

int replier_main(int domain_id)
{
    /* Uncomment this to turn on additional logging
        rti::config::Logger::instance().verbosity(
                rti::config::Verbosity::WARNING);
    */

    std::cout << "PrimeNumberReplier running (on domain " << domain_id << ")"
              << std::endl;

    try {
        PrimeNumberReplierExample(domain_id).run_example();
    } catch (const std::exception &ex) {
        std::cout << "Exception: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int domain_id = 0;

    if (argc > 1) {
        domain_id = atoi(argv[1]);
    }

    return replier_main(domain_id);
}
