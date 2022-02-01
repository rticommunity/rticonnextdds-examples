# Example Code: MultiChannel

If you haven't used the RTI Connext C# API before, first check the
[Getting Started Guide](https://community.rti.com/static/documentation/connext-dds/6.1.0/doc/manuals/connext_dds_professional/getting_started_guide/index.html).

## Building the Example :wrench:

First use **rtiddsgen** to generate the C# type and project files from
`Primes.idl`:

```sh
<install dir>/bin/rtiddsgen -language c# -platform net5 -create typefiles -create makefiles Primes.idl
```

Where `<install dir>` refers to your RTI Connext installation.

You will see some messages that look like:

```plaintext
WARN com.rti.ndds.nddsgen.emitters.FileEmitter File exists and will not be
overwritten
```

This is normal and is only informing you that some files that rtiddsgen
generates are already available and have been modified.

Then build it using the dotnet CLI:

```sh
dotnet build
```

## Running the Example

In two separate command prompt windows for the requester and replier. Run the
following commands from the example directory (this is necessary to ensure the
application loads the QoS defined in *USER_QOS_PROFILES.xml*):

```sh
dotnet run -- --replier
dotnet run -- --requester
```

For the full list of arguments:

```sh
dotnet run -- -h
```

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
$ dotnet run -- --requester -n 100
Running PrimesRequester on domain 0
2 3 5 7 11
13 17 19 23 29
31 37 41 43 47
53 59 61 67 71
73 79 83 89 97
DONE
```

Each line of numbers is an individual reply. You can set `--primes-per-reply`
to control this behavior in the example (by default it's 5).

```sh
$ dotnet run -- --requester -n 100 --primes-per-reply 10
Running PrimesRequester on domain 0
2 3 5 7 11 13 17 19 23 29
31 37 41 43 47 53 59 61 67 71
73 79 83 89 97 DONE
```

This demonstrates how a single request can be served by multiple replies.