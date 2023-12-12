import subprocess

# Self Signed CA
subprocess.run(["openssl", "req", "-nodes", "-x509", "-days", "1825", "-text", "-sha256", "-newkey", "ec", "-pkeyopt", "ec_paramgen_curve:prime256v1", "-keyout", "ca/private/CaKey.pem", "-out", "ca/CaCert.pem", "-config", "ca/Ca.cnf"])

# Generate Certs
subprocess.run(["openssl", "req", "-nodes", "-new", "-newkey", "rsa:2048", "-config", "identities/ParticipantA/ParticipantA.cnf", "-keyout", "identities/ParticipantA/ParticipantAKey.pem", "-out", "identities/ParticipantA/ParticipantA.csr"])
subprocess.run(["openssl", "x509", "-req", "-days", "730", "-text", "-CAserial", "ca/database/CaSerial", "-CA", "ca/CaCert.pem", "-CAkey", "ca/private/CaKey.pem", "-in", "identities/ParticipantA/ParticipantA.csr", "-out", "identities/ParticipantA/ParticipantA.pem"])

subprocess.run(["openssl", "req", "-nodes", "-new", "-newkey", "rsa:2048", "-config", "identities/ParticipantB/ParticipantB.cnf", "-keyout", "identities/ParticipantB/ParticipantBKey.pem", "-out", "identities/ParticipantB/ParticipantB.csr"])
subprocess.run(["openssl", "x509", "-req", "-days", "730", "-text", "-CAserial", "ca/database/CaSerial", "-CA", "ca/CaCert.pem", "-CAkey", "ca/private/CaKey.pem", "-in", "identities/ParticipantB/ParticipantB.csr", "-out", "identities/ParticipantB/ParticipantB.pem"])

# Signing XMLs with S/MIME
subprocess.run(["openssl", "smime", "-sign", "-in", "xml/governance_lws.xml", "-text", "-out", "xml/signed/signed_governance_lws.p7s", "-signer", "ca/CaCert.pem", "-inkey", "ca/private/CaKey.pem"])
subprocess.run(["openssl", "smime", "-sign", "-in", "xml/permissions.xml", "-text", "-out", "xml/signed/signed_permissions.p7s", "-signer", "ca/CaCert.pem", "-inkey", "ca/private/CaKey.pem"])