#
# (c) 2024 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software solely for use with RTI products.  The Software is
# provided "as is", with no warranty of any type, including any warranty for
# fitness for any purpose. RTI is under no obligation to maintain or support
# the Software.  RTI shall not be liable for any incidental or consequential
# damages arising out of the use or inability to use the software.
#

import time
import argparse
import rti.connextdds as dds
import rti.logging.distlog as distlog

def distlogger_example(application_kind, domain_id, sleep, iterations):
    # First, create the options to personalize Distributed Logger.
    # If no options are provided, default ones will be created.
    options = distlog.LoggerOptions()
    options.domain_id = domain_id
    options.application_kind = application_kind

    # Then, set the created options. The method init must be called before
    # using the Logger instance.
    distlog.Logger.init(options)

    # The log messages are published as DDS topics, which allows your DDS
    # applications to subscribe to them. You can also run rtiddsspy or
    # RTI Admin Console to visualize the logs.
    for i in range(1, iterations + 1):
        print(f"\nIteration #{i}")

        distlog.Logger.debug("This is a debug message")
        distlog.Logger.warning("This is a warning message")
        distlog.Logger.error("This is an error message")

        time.sleep(sleep)

    distlog.Logger.finalize()


def main():
    # Parse command line arguments
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-d", "--domain_id", type=int, default=0, help="Domain ID (default: 0)"
    )
    parser.add_argument(
        "-s",
        "--sleep",
        type=int,
        default=1,
        help="Seconds between iterations (default: 1)",
    )
    parser.add_argument(
        "-i",
        "--iterations",
        type=int,
        default=50,
        help="Number of iterations (default: 50)",
    )
    args = parser.parse_args()

    try:
        distlogger_example(
            "Python Distributed Logger Example",
            args.domain_id,
            args.sleep,
            args.iterations,
        )
    except Exception as ex:
        print(f"Exception in distlogger_example: {ex}")

    dds.DomainParticipant.finalize_participant_factory()


if __name__ == "__main__":
    main()
