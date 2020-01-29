######################################################################
# (c) Copyright, Real-Time Innovations, 2020.  All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or
# revisions thereof, must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.
#
# Makefile to create example certificates and keys
#
########################################################################

# Definitions

OPENSSL := openssl

INTERMEDIATE_DAYS      = 1825 # 5 years, no leap days
ROOT_DAYS              = 1825 # 5 years, no leap days
PEER_DAYS              = 730  # 2 years, no leap days


PKI_PREFIX = ecdsa01

ROOT_CA_NAME           = $(PKI_PREFIX)RootCa

ROOT_CA                = ca/$(ROOT_CA_NAME)

PEER01    = identities/$(PKI_PREFIX)Peer01
PEER02    = identities/$(PKI_PREFIX)Peer02

CERTIFICATE_LIST = \
				   $(ROOT_CA)Cert.pem \
				   $(PEER01)Cert.pem \
				   $(PEER02)Cert.pem


# Certificate generation

all: $(ROOT_CA_NAME).pki
	$(MAKE) cert

cert: $(CERTIFICATE_LIST)

include ../cert.mk


# CA certificates

# RootCa is self-signed
$(ROOT_CA)Cert.pem:
	$(OPENSSL) ecparam -name prime256v1 -out ca/private/$(ROOT_CA_NAME)Param
	$(OPENSSL) req -nodes -x509 -days $(ROOT_DAYS) -text -sha256 -newkey ec:ca/private/$(ROOT_CA_NAME)Param -keyout ca/private/$(ROOT_CA_NAME)Key.pem -out $(ROOT_CA)Cert.pem -config $(ROOT_CA).cnf


# Peer certificates

# RootCa signs Peer01Cert
$(PEER01)Cert.pem: $(ROOT_CA)Cert.pem
	$(OPENSSL) ecparam -name prime256v1 -out $(PEER01)Param
	$(OPENSSL) req -nodes -new -newkey ec:$(PEER01)Param -config $(PEER01).cnf -keyout $(PEER01)Key.pem -out $(PEER01).csr
	$(OPENSSL) ca -batch -create_serial -config $(ROOT_CA).cnf -days $(PEER_DAYS) -in $(PEER01).csr -out $(PEER01)Cert.pem

# RootCa signs Peer02Cert
$(PEER02)Cert.pem: $(ROOT_CA)Cert.pem
	$(OPENSSL) ecparam -name prime256v1 -out $(PEER02)Param
	$(OPENSSL) req -nodes -new -newkey ec:$(PEER02)Param -config $(PEER02).cnf -keyout $(PEER02)Key.pem -out $(PEER02).csr
	$(OPENSSL) ca -batch -create_serial -config $(ROOT_CA).cnf -days $(PEER_DAYS) -in $(PEER02).csr -out $(PEER02)Cert.pem

