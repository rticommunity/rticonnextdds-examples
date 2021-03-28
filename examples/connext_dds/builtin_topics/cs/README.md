# Example Code: Builtin Topics
If you haven't used the RTI Connext C# API before, first check the
[Getting Started Guide](https://community.rti.com/static/documentation/connext-dds/6.1.0/doc/manuals/connext_dds_professional/getting_started_guide/index.html).

## Building the Example :wrench:

First use **rtiddsgen** to generate the C# type and project files from
`BuiltinTopicsExample.idl`:

```sh
<install dir>/bin/rtiddsgen -language c# -platform net5 -create typefiles -create makefiles BuiltinTopicsExample.idl
```

Where `<install dir>` refers to your RTI Connext installation.

Then build it using the dotnet CLI:

```sh
dotnet build
```

## Running C# Example

In separate command prompt windows run the following commands from the example directory:

```sh
dotnet run -- --pub --password <required password>
dotnet run -- --sub --password <password >
```

The application acceps several arguments:

1. `--pub` or `--sub` run the publisher or subscriber application.

2. `--password <value>` specifies the password. In the publisher application
   this is the required password to allow subscribers to receive the published
   data. In the subscriber application this is the password used to authenticate
   with the publisher.

For the full list of arguments:

```sh
dotnet run -- -h
```

You can run more than one subscriber with different passwords to see how only
when the subscriber sends the right password the publisher allows it to
receive data.

The following is an example execution:

Publisher:

```plaintext
$ dotnet run -- --pub --password hello
Running HelloWorldPublisher on domain 0
Writing HelloWorld, count 0
Writing HelloWorld, count 1
Writing HelloWorld, count 2
Discovered participant: [Key = 20-B4-01-01-79-B5-48-B8-8D-36-F5-B8-C1-01-00-00, DomainId = 0, ProductVersion = 6.1.0.0, ...]
    * Authorization DENIED
Writing HelloWorld, count 3
Writing HelloWorld, count 4
Writing HelloWorld, count 5
Writing HelloWorld, count 6
Writing HelloWorld, count 7
Discovered participant: [Key = 7A-88-01-01-E0-E5-4F-89-87-14-5F-A3-C1-01-00-00, DomainId = 0, ProductVersion = 6.1.0.0, ...]
    * Authorization OK
Discovered subscription: [Key = 7A-88-01-01-E0-E5-4F-89-87-14-5F-A3-04-00-00-80, ParticipantKey = 7A-88-01-01-E0-E5-4F-89-87-14-5F-A3-C1-01-00-00, TopicName = Example msg, TypeName = HelloWorld, ...]
Writing HelloWorld, count 8
Writing HelloWorld, count 9
Writing HelloWorld, count 10
```

Subscriber 1 (using incorrect password):

```plaintext
$ dotnet run -- --sub --password bad
Running HelloWorldSubscriber on domain 0
```

Subscriber 2 (using correct password):

```plaintext
$ dotnet run -- --sub --password hello
Running HelloWorldSubscriber on domain 0
   x: 8

   x: 9

   x: 10
```
