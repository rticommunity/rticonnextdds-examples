# Tutorial: Distributed data cache

This code is part of the Connext Distributed data cache tutorial.

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

Extra. Running interactive publisher
```sh
dotnet run -- interactive_pub
...
```

Extra. Running interactive subscriber
```sh
dotnet run -- interactive_sub
...
```
