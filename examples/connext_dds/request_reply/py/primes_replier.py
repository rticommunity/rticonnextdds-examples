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
import math
from typing import Sequence, Tuple

import rti.connextdds as dds
from rti.request import Replier
import Primes


def is_prime(val):
    if val <= 1:
        return False
    if val == 2:
        return True
    if val > 2 and val % 2 == 0:
        return False

    max_div = int(math.floor(math.sqrt(val)))
    for i in range(3, 1 + max_div, 2):
        if val % i == 0:
            return False
    return True


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

    replier = Replier(
        request_type=Primes.PrimeNumberRequest,
        reply_type=Primes.PrimeNumberReply,
        participant=participant,
        service_name="PrimeCalculator",
        datawriter_qos=writer_qos,
        datareader_qos=reader_qos,
    )

    print(f"Prime calculation replier started on domain {domain_id}")

    max_wait = dds.Duration.from_seconds(20)
    requests: Sequence[
        Tuple[Primes.PrimeNumberRequest, dds.SampleInfo]
    ] = replier.receive_requests(max_wait)

    while len(requests) > 0:
        for request, request_info in requests:
            if not request_info.valid:
                continue

            if request.n <= 0 or request.primes_per_reply <= 0:
                error_reply = Primes.PrimeNumberReply(
                    status=Primes.PrimeNumberCalculationStatus.REPLY_ERROR
                )
                replier.send_reply(error_reply, request_info)
            else:
                n = request.n
                print(f"Calculating prime numbers below {n}...")

                max_count = request.primes_per_reply
                primes = dds.Int32Seq()

                reply = Primes.PrimeNumberReply()
                for m in range(1, n + 1):
                    if is_prime(m):
                        primes.append(m)
                        if len(primes) == max_count:
                            reply.primes = primes
                            reply.status = (
                                Primes.PrimeNumberCalculationStatus.REPLY_IN_PROGRESS
                            )
                            replier.send_reply(
                                reply, request_info, final=False
                            )
                            primes.clear()

                reply.primes = primes
                reply.status = (
                    Primes.PrimeNumberCalculationStatus.REPLY_COMPLETED
                )
                replier.send_reply(reply, request_info)

                print("DONE")

        requests = replier.receive_requests(max_wait)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="RTI Connext DDS Example: Request-Reply (Replier)"
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

    try:
        replier_main(args.domain)
    except dds.TimeoutError:
        print("Timeout: no requests received")
