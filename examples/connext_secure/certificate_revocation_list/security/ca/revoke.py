import subprocess

# Revoke
subprocess.run(
    [
        "openssl",
        "ca",
        "-config",
        "Ca.cnf",
        "-batch",
        "-revoke",
        "../identities/ParticipantB/ParticipantB.pem",
    ]
)

# Generate CRL
subprocess.run(
    ["openssl", "ca", "-config", "Ca.cnf", "-batch", "-gencrl", "-out", "CaCrl.crl"]
)
