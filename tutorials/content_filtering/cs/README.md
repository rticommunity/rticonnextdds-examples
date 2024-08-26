# Tutorial: Content Filtering

This code is part of the Connext
[Content Filtering](https://community.rti.com/static/documentation/developers/learn/content-filtering.html)
tutorial.

## Building the Example :wrench:

You can build the example following the instructions in the tutorial, or you can
build it using CMake as follows.

1.  Generate the C# types and the project files file with **rtiddsgen**:

```sh
<install dir>/bin/rtiddsgen -language C# -platform [net5|net6|net8] home_automation.idl
```

Where `<install dir>` refers to your RTI Connext installation.

2.  Build the applications:

```sh
dotnet build
```

## Running the Example :rocket:

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

(Optional) Run the subscriber that updates the filter
```sh
dotnet run -- filter
...
```