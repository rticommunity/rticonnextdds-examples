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
from typing import Sequence

import rti.connextdds as dds
from rti.request import Requester
import Primes


def requester_main(domain_id, n, primes_per_reply):
    # Get the QoS from a profile in USER_QOS_PROFILES.xml (the default
    # QosProvider will load the USER_QOS_PROFILES.xml file from the current
    # working directory)
    qos_provider = dds.QosProvider.default
    writer_qos = qos_provider.datawriter_qos_from_profile(
        "RequestReplyExampleProfiles::RequesterExampleProfile"
    )
    reader_qos = qos_provider.datareader_qos_from_profile(
        "RequestReplyExampleProfiles::RequesterExampleProfile"
    )

    participant = dds.DomainParticipant(domain_id)

    requester = Requester(
        request_type=Primes.PrimeNumberRequest,
        reply_type=Primes.PrimeNumberReply,
        participant=participant,
        service_name="PrimeCalculator",
        datawriter_qos=writer_qos,
        datareader_qos=reader_qos,
    )

    print(f"Waiting to discover replier on domain {domain_id}")

    while requester.matched_replier_count == 0:
        time.sleep(0.1)

    prime_number_request = Primes.PrimeNumberRequest(
        n=n, primes_per_reply=primes_per_reply
    )

    print(
        f"Sending a request to calculate the prime numbers <= {n} in sequences of {primes_per_reply} or fewer elements"
    )

    request_id = requester.send_request(prime_number_request)

    max_wait = dds.Duration.from_seconds(20)
    in_progress = True
    while in_progress:
        if not requester.wait_for_replies(
            max_wait, related_request_id=request_id
        ):
            raise dds.TimeoutError("Timed out waiting for replies")

        # It's possible to call requester.take_replies(), but that method also
        # returns SampleInfo objects, which we don't need here.
        replies: Sequence[
            Primes.PrimeNumberReply
        ] = requester.reply_datareader.take_data()
        for reply in replies:
            print(list(reply.primes))

            if (
                reply.status
                != Primes.PrimeNumberCalculationStatus.REPLY_IN_PROGRESS
            ):
                in_progress = False
                if (
                    reply.status
                    == Primes.PrimeNumberCalculationStatus.REPLY_ERROR
                ):
                    raise RuntimeError("Error in Replier")

    print("DONE")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="RTI Connext DDS Example: Request-Reply (Requester)"
    )
    parser.add_argument(
        "-d",
        "--domain",
        type=int,
        default=0,
        help="DDS Domain ID (default: 0)",
    )
    parser.add_argument(
        "-p",
        "--primes-per-reply",
        type=int,
        default=5,
        help="Number of primes per reply (min: 5, default: 5)",
    )
    parser.add_argument(
        "n", type=int, help="Request will retrieve prime numbers <= n"
    )

    args = parser.parse_args()
    assert 0 <= args.domain < 233
    assert args.primes_per_reply >= 5

    requester_main(args.domain, args.n, args.primes_per_reply)
