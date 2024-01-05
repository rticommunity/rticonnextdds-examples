## Multicast example

export NDDSHOME=~/rti_connext_dds-7.2.0
export NDDS_QOS_PROFILES=qos.xml
export SHMEM_DOMAIN=0
export UDP_DOMAIN=1
export APPS=20

## Multicast-less example (CDS)

export NDDSHOME=~/rti_connext_dds-7.2.0
export NDDS_QOS_PROFILES=qos.xml
export SHMEM_DOMAIN=0
export UDP_DOMAIN=1
export APPS=20
export CDS_IP_ADDRESS=127.0.0.1
export CDS_PORT=8000