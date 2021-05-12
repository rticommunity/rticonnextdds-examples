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

import requests


def read_or_take(data_reader_url, on_data_available_fnc, take):
    """Performs a read or take operation on a DDS DataReader using
    long-polling. If new data becomes available, the function automatically
    wakes up and calls a given on_data_available_fnc to handle those new
    samples.
    """
    # Timeout in Seconds
    max_wait = 30

    # Do HTTP request, which calls read_or_take again upon completion
    # We reuse the session to avoid closing the underlying TCP connection.
    # Remeber to execute Web Integration Service with
    # -enableKeepAlive yes and an appropriate -keepAliveTimeout time.
    req_session = requests.Session()

    while True:
        # We call GET and use JSON format, take or read depending on the
        # take parameter, and a maxWait timeout of 30 seconds, which is the
        # time Web Integration Service's internal waitset will block until
        # there are samples with sampleStateMask = "NOT_READ."
        # Note that this configuration will work well for only one client
        # at a time, because as any other client calls read the sample
        # status will change. Therefore, with this configuration, different
        # clients need to use different DataReaders.
        response = req_session.get(
            data_reader_url,
            params={
                "sampleFormat": "json",
                "removeFromReaderCache": "true" if take else "false",
                "maxWait": str(max_wait),
                "sampleStateMask": "NOT_READ",
            },
            timeout=(max_wait * 2),
        )
        if (response.status_code == 200) and (response.text != "[]"):
            # Only call on_data_available if the status code was 200, and
            # we did not get an empty sequence of samples.
            on_data_available_fnc(response.text)


def read(data_reader_url, on_data_available_fnc):
    """Performns a read() operation on a DDS DataReader given its resource
    URL. The operation performs a long-polling operation that calls the
    given on_data_available_fnc whenever new samples become available.
    As opposed to the take() operation, the read() operation does not remove
    the samples it has read from the DataReaders cache.
    """
    read_or_take(data_reader_url, on_data_available_fnc, False)


def take(data_reader_url, on_data_available_fnc):
    """Performns a take() operation on a DDS DataReader given its resource
    URL. The operation performs a long-polling operation that calls the
    given on_data_available_fnc whenever new samples become available.
    After reading the available samples, the take() operation removes all data
    in the DataReaders cache.
    """
    read_or_take(data_reader_url, on_data_available_fnc, True)
