# Example Code: Request-Reply

If you haven't used the RTI Connext Python API before, first check the
[Getting Started Guide](https://community.rti.com/static/documentation/connext-dds/7.0.0/doc/manuals/connext_dds_professional/getting_started_guide/index.html).

## Running the Example

In two separate command prompt windows for the requester and replier run the
following commands from the example directory (this is necessary to ensure the
application loads the QoS defined in *USER_QOS_PROFILES.xml*):

```sh
python primes_replier.py
python primes_requester.py <n>
```

Where ``<n>`` is the number to calculate the primes below.

Use ``-h`` to see the full list of arguments.

## Replier Output

The replier will print a message every time it receives a request, for example:

```plaintext
Calculating prime numbers below 100...
DONE
```

## Requester Output

The requester prints the prime numbers sent by the replier, as they are received.

For example:

```sh
$ python primes_requester.py 100
Sending a request to calculate the prime numbers <= 100 in sequences of 5 or fewer elements
[2, 3, 5, 7, 11]
[13, 17, 19, 23, 29]
[31, 37, 41, 43, 47]
[53, 59, 61, 67, 71]
[73, 79, 83, 89, 97]
DONE
```

Each line of numbers is an individual reply. You can set `--primes-per-reply`
to control this behavior in the example (by default it's 5).

```sh
$ python primes_requester.py 100 --primes-per-reply 10
Running PrimesRequester on domain 0
Sending a request to calculate the prime numbers <= 100 in sequences of 10 or fewer elements
[2, 3, 5, 7, 11, 13, 17, 19, 23, 29]
[31, 37, 41, 43, 47, 53, 59, 61, 67, 71]
[73, 79, 83, 89, 97]
DONE
```

This demonstrates how a single request can be served by multiple replies.
