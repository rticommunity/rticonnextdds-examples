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
        print(
            f"\nTotal publishers: {matched_status.current_count}, "
            + f"Change: {matched_status.current_count_change}"
        )

        pub_handles = reader.matched_publications

        i = 0
        for pub_handle in pub_handles:
            builtin_data = reader.matched_publication_data(pub_handle)
            print(f"Publisher {i}:")
            print(f"    Publisher Virtual GUID: {builtin_data.virtual_guid}")
            print(f"    Publisher Topic: {builtin_data.topic_name}")
            print(f"    Publisher Type: {builtin_data.type_name}")
            print(f"    Publisher Name: {builtin_data.publication_name.name}")
            i += 1

    status_condition = dds.StatusCondition(reader)
    status_condition.enabled_statuses = dds.StatusMask.SUBSCRIPTION_MATCHED
    status_condition.set_handler(handler)

    wait_set = dds.WaitSet()
    wait_set += status_condition

    while True:
        wait_set.dispatch(4.0)


if __name__ == "__main__":
    try:
        sensor_monitoring()
    except KeyboardInterrupt:
        pass
