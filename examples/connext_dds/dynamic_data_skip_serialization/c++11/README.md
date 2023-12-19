# Example Code: Skip Data Serialization for Data Recording

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

## Running the Example

From the build directory, in one command prompt run the recorder:

```sh
./recorder --record data.bin
```

In a second command prompt run the publisher:

```sh
./recorder --publish
```

The recorder application will print a message each time a sample is recorded.

Now kill the recorder and run the replay application. A file called `data.bin`
will have been created in the current directory.

Now we will run a subscriber and a reply application.

To subscribe to the data we will simply use **rtiddsspy**:

```sh
<Connext installation>/bin/rtiddsspy -printSample
```

Now run the application that replays the data recorded in  `data.bin`:

```sh
./recorder --replay data.bin
```

The subscriber will print the data that is being replayed.

