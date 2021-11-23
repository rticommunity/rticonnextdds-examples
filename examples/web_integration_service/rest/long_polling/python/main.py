#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# (c) 2017 Copyright, Real-Time Innovations, Inc.  All rights reserved.
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software.  Licensee has the right to distribute object form
# only for use with RTI products.  The Software is provided "as is", with no
# warranty of any type, including any warranty for fitness for any purpose.
# RTI is under no obligation to maintain or support the Software.  RTI shall
# not be liable for any incidental or consequential damages arising out of the
# use or inability to use the software.

import long_polling


def on_data_available(data):
    """Handles data received by the read or take functions whenever data is
    available. In this case, the function prints out the data it receives,
    which is string represented in JSON format.
    """
    print(data)


def main():
    # Assume that we are running on localhost
    host = "localhost"
    port = 8080

    # Prepare request
    datareader_url = (
        f"http://{host}:{port}"
        "/dds/rest1"
        "/applications/ShapesDemoApp"
        "/domain_participants/MyParticipant"
        "/subscribers/MySubscriber"
        "/data_readers/MySquareReader"
    )

    print("Starting example...")

    # The take function will block, if you want to continue your execution
    # then take and subsequent calls to on_data_available need to be run on
    # a separate thread.
    try:
        long_polling.take(datareader_url, on_data_available)
    except (KeyboardInterrupt, SystemExit):
        pass
    except Exception as e:
        print(e)
        pass

    print("Stopping example...")


if __name__ == "__main__":
    main()
