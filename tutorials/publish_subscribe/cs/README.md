# Tutorial: Publish-Subscribe

This code is part of the Connext Publish-Subscribe tutorial.

## Building the Example :wrench:

1. Generate the C# types and the project files file with **rtiddsgen**:

```sh
<install dir>/bin/rtiddsgen -language C# -platform [net5|net6|net8] home_automation.idl
```

Where `<install dir>` refers to your RTI Connext installation.

2. Build the applications:

```sh
dotnet build
```

## Running the Applications :rocket:

Running publisher
```sh
dotnet run -- pub
...
```

Running subscriber
```sh
dotnet run -- sub
...
```

Extra. Running subscriber with timestamp
```sh
dotnet run -- sub_timestamp
...
```

