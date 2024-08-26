# Tutorial: Publish-Subscribe

This code is part of the Connext
[Publish-Subscribe](https://community.rti.com/static/documentation/developers/learn/publish-subscribe.html)
tutorial.

## Building the Example :wrench:

1.  Generate the C# types and the project files file with **rtiddsgen**:

```sh
<install dir>/bin/rtiddsgen -language C# -platform [net5|net6|net8] home_automation.idl
```

Where `<install dir>` refers to your RTI Connext installation.

2.  Build the applications:

```sh
dotnet build
```

## Running the Applications :rocket:

Run the publisher

```sh
dotnet run -- pub
...
```

Run the subscriber

```sh
dotnet run -- sub
...
```

Extra. Run the subscriber with timestamp

```sh
dotnet run -- sub_timestamp
...
```
