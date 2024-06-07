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

import rti.connextdds as dds
from home_automation import DeviceStatus


def sensor_monitoring():
    participant = dds.DomainParticipant(domain_id=0)
    topic = dds.Topic(participant, "WindowStatus", DeviceStatus)
    reader = dds.DataReader(topic)

    def handler(_):
        matched_status = reader.subscription_matched_status
        if matched_status.current_count_change > 0:
            print(f"Publisher matched (Total {matched_status.current_count})")
        else:
            print(f"Publisher unmatched (Total {matched_status.current_count})")

        pubs = reader.matched_publications
        pub_names = [
            reader.matched_publication_data(pub).publication_name.name for pub in pubs
        ]
        print(f"Matched Publishers = {pub_names}\n")

    status_condition = dds.StatusCondition(reader)
    status_condition.enabled_statuses = dds.StatusMask.SUBSCRIPTION_MATCHED
    status_condition.set_handler(handler)

    waitset = dds.WaitSet()
    waitset += status_condition

    while True:
        waitset.dispatch(4.0)


if __name__ == "__main__":
    try:
        sensor_monitoring()
    except KeyboardInterrupt:
        pass
