#
# (c) 2023 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software solely for use with RTI products.  The Software is
# provided "as is", with no warranty of any type, including any warranty for
# fitness for any purpose. RTI is under no obligation to maintain or support
# the Software.  RTI shall not be liable for any incidental or consequential
# damages arising out of the use or inability to use the software.
#

import argparse
import time
import math

import rti.connextdds as dds
from rti.request import SimpleReplier
import Primes

request_serviced = True


def is_prime(val):
    if val <= 1:
        return False
    if val == 2:
        return True
    if val > 2 and val % 2 == 0:
        return False

    max_div = math.floor(math.sqrt(val))
    for i in range(3, 1 + max_div, 2):
        if val % i == 0:
            return False
    return True


request_serviced = True

def request_handler(request: Primes.PrimeNumberRequest):
    global request_serviced
    request_serviced = True
    if request.n <= 0 or request.primes_per_reply <= 0:
        error_reply = Primes.PrimeNumberReply(
            status=Primes.PrimeNumberCalculationStatus.REPLY_ERROR
        )
        return error_reply
    else:
        n = request.n
        print(f"Calculating prime numbers below {n}...")

        max_count = request.primes_per_reply
        primes = dds.Int32Seq()

        reply = Primes.PrimeNumberReply()
        for m in range(1, n + 1):
            if is_prime(m):
                primes.append(m)
                if len(primes) > max_count:
                    reply.status = (
                        Primes.PrimeNumberCalculationStatus.REPLY_ERROR
                    )
                    print("Error: too many primes for a single reply")
                    return reply

        reply.primes = primes
        reply.status = Primes.PrimeNumberCalculationStatus.REPLY_COMPLETED
        print("DONE")

        return reply


def replier_main(domain_id):
    # Get the QoS from a profile in USER_QOS_PROFILES.xml (the default
    # QosProvider will load the USER_QOS_PROFILES.xml file from the current
    # working directory)
    qos_provider = dds.QosProvider.default
    writer_qos = qos_provider.datawriter_qos_from_profile(
        "RequestReplyExampleProfiles::ReplierExampleProfile"
    )
    reader_qos = qos_provider.datareader_qos_from_profile(
        "RequestReplyExampleProfiles::ReplierExampleProfile"
    )

    participant = dds.DomainParticipant(domain_id)

    replier = SimpleReplier(
        request_type=Primes.PrimeNumberRequest,
        reply_type=Primes.PrimeNumberReply,
        participant=participant,
        service_name="PrimeCalculator",
        handler=request_handler,
        datawriter_qos=writer_qos,
        datareader_qos=reader_qos,
    )
    print(f"Prime calculation replier started on domain {domain_id}...")

    global request_serviced
    while request_serviced:
        request_serviced = False
        time.sleep(20)

    print("Timed out waiting for requests")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="RTI Connext DDS Example: Request-Reply (Simple Replier)"
    )
    parser.add_argument(
        "-d",
        "--domain",
        type=int,
        default=0,
        help="DDS Domain ID (default: 0)",
    )

    args = parser.parse_args()
    assert 0 <= args.domain < 233

    replier_main(args.domain)
