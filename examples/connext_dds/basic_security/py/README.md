# Example Code: Basic Security

## Running this Example

## Initial Setup

In order to run this example you must have security certificates. The easiest
way to acquire them is to copy them from the rti_workspace examples.
They can be found in `<rti_workspace>/<version>/examples/dds_security`.
If this folder is copied in whole to examples/connext_dds/basic_security the
example will work with no modifications to the SecureQos.xml file.
Otherwise, the strings in SecureQos.xml must be updated to reflect the locations
of the certificates.

## Linux and macOS systems

Your LD_LIBRARY_PATH or (DYLD_LIBRARY_PATH for macOS) must include
`$NDDSHOME/lib/<architecture>`.
You must also include the path to your crypto libraries. They are in
`$NDDSHOME/third_party/openssl-<version>/<architecture>/<release or debug>/lib`
(location of libcrypto.so and libssl.so).
If you are using the Security Plugins for use with wolfSSL, your
LD_LIBRARY_PATH must include
`$NDDSHOME/third_party/wolfssl-<version>/<architecture>/<release or debug>/lib`
(location of libwolfssl.so).

If using Certicom Security Builder Engine for QNX, your LD_LIBRARY_PATH must
include
`$NDDSHOME/third_party/openssl-<version>/<architecture>/release/lib/:$CERTICOM_SBENGINEHOME/tools/sb/sb-$CERTICOMOS/lib/:$CERTICOM_SBENGINEHOME/lib/$CERTICOMOS.`

To run this example, type the following commands in two different command
shells (one command in each shell), either on the same machine or on different
machines:

  > python3 secure_pub.py
  > python3 secure_sub.py

## Windows systems

Your PATH must include
`%NDDSHOME%\lib\<architecture>` and
`%NDDSHOME%\third_party\openssl-<version>\<architecture>\<release or debug>\bin`
(location of the libcrypto and libssl DLLs).

To run this example, type the following commands in two different command
shells (one command in each shell), either on the same machine or on different
machines:

  > python3 secure_pub.py
  > python3 secure_sub.py

## Accepted parameters

The following parameters are accepted:

- `-d` for the domain id
- `-c` for the number of samples to send
- `-p` for the profile (A, B, RSA_A, RSA_B, ECDSA_P384_A, or ECDSA_P384_B)

To run this example
using the "rsa"(**) algorithm suite, add a `-p` command-line argument: "rsa".
Pass "p384" (***) for the `-p` command-line argument if you want to use the
"ecdsa secp384r1" algorithm suite.

-   The "ecdsa" algorithm suite consists of ECDSA+P256+SHA256 (digital signature
for identity trust chain and authentication) and ECDHE-CEUM+P256
(key establishment).
-   The "rsa" algorithm suite consists of RSASSA-PKCS1-V1_5+2048+SHA256 (digital
signature for identity trust chain), RSASSA-PSS-MGF1SHA256+2048+SHA256
(digital signature for authentication) and ECDHE-CEUM+P256 (key establishment).
-   The "ecdsa secp384r1" algorithm suite consists of ECDSA+P384+SHA384 (digital
signature for identity trust chain and authentication), and ECDHE-CEUM+P384
(key establishment).

## Troubleshooting this Example

If you see the following error:

`Cryptography_encrypt:error:0607B083:lib(6):func(123):reason(131)`

you are likely using an older version of OpenSSL. You need version
1.0.1c or later.

For more information, please consult the "RTI Security Plugins Getting Started
Guide".
