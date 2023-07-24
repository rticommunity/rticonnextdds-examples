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


def calculate_and_send_primes(
    replier: Replier,
    request: Primes.PrimeNumberRequest,
    request_info: dds.SampleInfo,
):
    n = request.n
    primes_per_reply = request.primes_per_reply

    reply = Primes.PrimeNumberReply()
    reply.primes = dds.Int32Seq()
    reply.status = Primes.PrimeNumberCalculationStatus.REPLY_IN_PROGRESS

    # prime[i] indicates if i is a prime number
    # Initially, we assume all of them except 0 and 1 are
    prime = [True] * (n + 1)
    prime[0] = False
    prime[1] = False

    m = int(math.sqrt(n))

    for i in range(2, m + 1):
        if prime[i]:
            for j in range(i * i, n + 1, i):
                prime[j] = False

            # Add a new prime number to the reply
            reply.primes.append(i)

            if len(reply.primes) == primes_per_reply:
                # Send a reply now
                replier.send_reply(reply, request_info, final=False)
                reply.primes.clear()

    # Calculation is done. Send remaining prime numbers
    for i in range(m + 1, n + 1):
        if prime[i]:
            reply.primes.append(i)

        if len(reply.primes) == primes_per_reply:
            replier.send_reply(reply, request_info, final=False)
            reply.primes.clear()

    # Send the last reply. Indicate that the calculation is complete and
    # send any prime number left in the sequence
    reply.status = Primes.PrimeNumberCalculationStatus.REPLY_COMPLETED
    replier.send_reply(reply, request_info)


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

            calculate_and_send_primes(replier, request, request_info)

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
