######################################################################
# (c) Copyright, Real-Time Innovations, 2020.  All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or
# revisions thereof, must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.
#
# Makefile utilities to generate example PKIs and certificates
#
########################################################################

# Makefile utilities

%.pki:
	mkdir -p ca/private ca/database ca/newCerts
	chmod 700 ca/private
	touch ca/database/$*Index
	echo 01 > ca/database/$*Crlnumber

clean: cleanTmpFiles
	-$(RM) -r ca/private ca/database ca/newCerts ca/*.pem ca/*.crl
	-$(RM) identities/*.pem

cleanTmpFiles:
	-$(RM) ca/private/*Param ca/database/* ca/newCerts/* ca/*.csr
	-$(RM) identities/*.csr identities/*Param

.PHONY: all %.pki clean cleanTmpFiles
.PRECIOUS:
