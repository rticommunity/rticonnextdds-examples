# Example Code: Remote Procedure Calls

:warning: **Note**: For an overview of this feature, see the
[RPC Tutorial](https://community.rti.com/static/documentation/connext-dds/7.2.0/doc/api/connext_dds/api_cpp2/group__RpcTutorialModule.html)
section of the *Connext* Modern C++ API Reference.

## Building the Example :wrench:

To build this example, first run CMake to generate the corresponding build
files. We recommend you use a separate directory to store all the generated
files (e.g., ./build).

```sh
mkdir build
cd build
cmake ..
```

Once you have run CMake, you will find a number of new files in your build
directory (the list of generated files will depend on the specific CMake
Generator). To build the example, run CMake as follows:

```sh
cmake --build .
```

**Note**: if you are using a multi-configuration generator, such as Visual
Studio solutions, you can specify the configuration mode to build as follows:

```sh
cmake --build . --config Release|Debug
```

Alternatively, you can use directly the generated infrastructure (e.g.,
Makefiles or Visual Studio Solutions) to build the example. If you generated
Makefiles in the configuration process, run make to build the example. Likewise,
if you generated a Visual Studio solution, open the solution and follow the
regular build process.
Running this Example

## Running the Example

This example is composed of two applications: a service and a client.

To run this example on a UNIX-based system, type the following in a command
shell:

```sh
build/Inventory_service
```

The client application can add or remove items to the inventory. On a new
shell run the following commands:

```sh
build/Inventory_client --add bananas
build/Inventory_client --remove apples --quantity 99
```

(Use ``-h`` to see the full list of arguments.)

## Expected output

The expected client output will be:

```sh
$ build/Inventory_client --add bananas

Initial inventory: [items: {[name: apples, quantity: 100], [name: oranges, quantity: 50]}]
Add 1 bananas
Updated inventory: [items: {[name: apples, quantity: 100], [name: bananas, quantity: 1], [name: oranges, quantity: 50]}]
```

```sh
$ build/Inventory_client --remove apples --quantity 99

Initial inventory: [items: {[name: apples, quantity: 100], [name: bananas, quantity: 1], [name: oranges, quantity: 50]}]
Remove 99 apples
Updated inventory: [items: {[name: apples, quantity: 1], [name: bananas, quantity: 1], [name: oranges, quantity: 50]}]
```

## Exception propagation

Trying to remove an item that is not in the inventory will propagate an
`UnknownItemError` exception to the client:

```sh
$ build/Inventory_client --remove pears

...
Unknown item: pears
...
```

## Concurrency

The example also demonstrates the service concurrency model. The service
application can be run with the `--delay <seconds>` argument. This makes the
*add* and *remove* operations sleep for the specified number of seconds,
simulating a long running operation.

Close the service from the previous section and start a new one:

```sh
build/Inventory_service --delay 10
```

On another window run a client:

```sh
$ build/Inventory_client --add bananas

Initial inventory: [items: {[name: apples, quantity: 100], [name: oranges, quantity: 50]}]
```

And on another window run another client:

```sh
$ build/Inventory_client --add bananas

Initial inventory: [items: {[name: apples, quantity: 100], [name: oranges, quantity: 50]}]
```

Note how the second client was able to get the inventory while the *add*
operation from the first client is still running.

After 10 seconds, the first client will finish:

```sh
Updated inventory: [items: {[name: apples, quantity: 100], [name: oranges, quantity: 50], [name: bananas, quantity: 1]}]
```

Followed by the second client:

```sh
Updated inventory: [items: {[name: apples, quantity: 100], [name: oranges, quantity: 50], [name: bananas, quantity: 2]}]
```

A service runs its operations in a thread pool; the number of threads can be
configured with the `dds::rpc::ServerParams` argument. The example sets the
number of threads to 4.
