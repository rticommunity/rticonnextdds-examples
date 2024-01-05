REM Multicast example

set NDDSHOME="C:/Program Files/rti_connext_dds-7.2.0"
set NDDS_QOS_PROFILES=qos.xml
set SHMEM_DOMAIN=0
set UDP_DOMAIN=1
set APPS=20

REM Multicast-less example (CDS)

set NDDSHOME="C:/Program Files/rti_connext_dds-7.2.0"
set NDDS_QOS_PROFILES=qos.xml
set SHMEM_DOMAIN=0
set UDP_DOMAIN=1
set APPS=20
set CDS_IP_ADDRESS=127.0.0.1
set CDS_PORT=8000