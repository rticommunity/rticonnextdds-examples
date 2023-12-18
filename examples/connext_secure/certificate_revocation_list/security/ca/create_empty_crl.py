import subprocess

# Generate CRL
subprocess.run(
    [
        "openssl",
        "ca",
        "-config",
        "Ca.cnf",
        "-batch",
        "-gencrl",
        "-out",
        "CaCrl.crl"
    ]
)
