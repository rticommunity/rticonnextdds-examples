#!/bin/bash

# Sleep to try to avoid DIT-194
# The replier run for 20 seconds, so we can sleep for 10 seconds
# to try to avoid the issue
sleep 10

cd /test/connextdds_examples_suite/connextdds_examples_repo/examples/connext_dds/request_reply/cs
/usr/bin/dotnet run -- --requester $@