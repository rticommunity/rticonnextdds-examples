# Example Code: Recording Service Administration

## Concept

*Recording Service* uses the common Remote Administration Platform (see [Remote
Administration
Platform](https://community.rti.com/static/documentation/connext-dds/6.0.0/doc/manuals/recording_service/common/remote_admin_platform.html))
This example shows how to use the platform to send remote commands to a running
instance of Recording Service. Note that this application can also work with a
running instance of *RTI Routing Service*, as the admin platform is common to
both).

## Example Description

This example shows a one-shot application that can be used to send one remote
command to a running application of Recording Service (you can also try to use
it with Routing Service). The application will prepare a command request based
on the command-line parameters provided in the application invocation. After
sending that request out to the service, it will wait for a command reply to be
sent by the service. The application will then exit.

The following files are part of the example:

-   `Requester.hpp` and `Requester.cxx`: these files contain the example C++
    code that prepares and sends the command request and then waits for a reply
    from the running service instance.

-   `USER_QOS_PROFILES.xml`: this file defines DDS XML QoS profiles to be used
    by the application and by *Recording Service*.

-   `recorder_remote_admin.xml`: this is a configuration file for Recording
    Service that enables remote administration, and uses the QoS profiles
    defined in the XML QoS profiles file defined above. By using the same QoS
    profiles in both the application and the *Recording Service* we ensure the
    QoS matching and the reliability of the communication.

-   `replay_remote_admin.xml`: this is a configuration file for Replay
    Service that enables remote administration, and uses the QoS profiles
    defined in the XML QoS profiles file defined above. By using the same QoS
    profiles in both the application and the *Replay Service* we ensure the
    QoS matching and the reliability of the communication.

## Building the Example

Build the example code by running the following command:

```sh
mkdir build
cd build
cmake ..
cmake --build .
```

> **Note**:
>
> When using a multi-configuration generator, make sure you specify
> the `--config` parameter in your call to `cmake --build .`. In general,
> it's a good practice to always provide it.

This will produce a binary directory (*build*) where the `Requester` application
can be found. The XML files in the source directory are also copied over to this
binary directory so that *Recording Service* can be run directly from this
directory as well.

> **Note:**
>
> When you need to cross-compile the example, the above
> command will not work, the assigned compiler won't be the cross-compiler and
> errors may happen when linking against the cross-compiled Connext binaries. To
> fix this, you have to create a file with the architecture name and call CMake
> with a specific flag called `-DCMAKE_TOOLCHAIN_FILE`. An example of the file to
> create with the toolchain settings (e.g. for an ARM architectures):

```cmake
set(CMAKE_SYSTEM_NAME Linux)
set(toolchain_path "<path to>/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian")
set(CMAKE_C_COMPILER "${toolchain_path}/bin/arm-linux-gnueabihf-gcc")
set(CMAKE_CXX_COMPILER "${toolchain_path}/bin/arm-linux-gnueabihf-g++")
```

Then you can call CMake like this:

```sh
cmake -DCONNEXTDDS_DIR=<connext dir> \
      -DCMAKE_TOOLCHAIN_FILE=<toolchain file created above> \
      -DCONNEXTDDS_ARCH=<connext architecture> ..
```

## Running the Example

First of all, we need a running *Recording Service* instance with remote
administration enabled. We will use the *Recording Service* configuration
shipped with the example, called `recorder_remote_admin.xml`:

```sh
cd <binary directory>
$(NDDSHOME)/bin/rtirecordingservice
        -cfgFile recorder_remote_admin.xml -cfgName remote_admin
```

Or in the case of *Replay Service*:

```sh
cd <binary directory>
$(NDDSHOME)/bin/rtireplayservice
        -cfgFile replay_remote_admin.xml -cfgName remote_admin
```

Note: you can run from other directory, just make sure the
`recorder_remote_admin.xml`, `replay_remote_admin.xml` and `USER_QOS_PROFILES.xml`

Once the *Recording Service* instance is running, it is ready to receive remote
command requests. The administration app receives the following command-line
parameters:

-   The kind of command to be sent, following a *CRUD* pattern (CREATE, READ,
    UPDATE, DELETE).

-   The identifier of the resource we're accessing, e.g.
    `recording_services/remote_admin/state` or
    `/replay_services/remote_admin/playback:continue`

-   [Optional] The arguments that the command kind needs for performing the
    action, e.g. `paused` (if the objective state of the service is *paused*)

-   [Optional] `--domain-id`: this parameter should be used when the *Recording
    Service* instance is running admninistration on a DDS domain different than
    0 (this can be achieved by modifying the XML configuration or using the
    `-remoteAdministrationDomainId` parameter in the invocation). The domain
    specified for this command-line parameter and the one used for remote
    administration of the service should be the same, otherwise the application
    won't be able to connect to the service.

-   [Optional] `--time-tag`: there are commands that are service-specific. This
    command-line parameter allows the user to define a timestamp tag for
    *Recording Service* to store. These tags can later be used to easily,
    symbolically define time ranges to be replayed or converted. This command
    requires a name parameter and optionally, a description (remember to use
    quotes for multi-word descriptions).

-   [Optional] `--add-breakpoint`: this command-line parameter allows the user
    to define a breakpoint for *Replay Service*. These breakpoints will be used
    in the debug mode of Replay. This command requires a timestamp in nanosecond
    form and optionally, a breakpoint name.

-   [Optional] `--remove-breakpoint`: it allows the user to remove an
    existing breakpoint of the replay. This command requires the timestamp or
    name of the breakpoint to be removed.

-   [Optional] `--goto-breakpoint`: this command-line parameter is used to jump
    to a existed breakpoint. This command requires the timestamp or name of the
    specific breakpoint.

-   [Optional] `--continue-seconds`: this command-line parameter allows the user
    to continue the replay for a specific amount of seconds. This command
    requires the number of seconds to be replayed.

-   [Optional] `--continue-slices`: it allows the user to continue the
    replay for a specific amount of slices. This command requires the number of
    slices to be replayed.

-   [Optional] `--current-timestamp`: it allows the user to jump in time
    forward and backward in *Replay Service*. This command requires a timestamp
    in nanosecond format.

Note: In order to run the *Replay Service* debug mode commands you must
uncomment the debug mode configuration inside `replay_remote_admin.xml`.

To run the administration app, you should go to the binary directory, or if you
run from other directories, make sure the `USER_QOS_PROFILES.xml` file is
present in the directory. We'll now show you some examples on how to send
different commands to the service.

1.  Sending a pause command:

    ```plaintext
    Requester UPDATE /recording_services/remote_admin/state paused
    ```

2.  Sending a timestamp tag command to a service with administration running on
    DDS domain ID 54:

    ```plaintext
    Requester UPDATE /recording_services/remote_admin/storage/sqlite:tag_timestamp --domain-id 54 --time-tag "A timestamp tag" "A timestamp tag description"
    ```

3.  Sending a shutdown command

    ```plaintext
    Requester DELETE /recording_services/remote_admin
    ```

4.  Sending an add breakpoint command

    ```plaintext
    Requester UPDATE /replay_services/remote_admin/playback:add_breakpoint --add-breakpoint 1613896947838268998 "breakpoint_1"
    ```

5.  Sending a Continue command

    ```plaintext
    Requester UPDATE /replay_services/remote_admin/playback:continue
    ```

## Customizing the Build

### Configuring Build Type and Generator

By default, CMake will generate build files using the most common generator for
your host platform (e.g., Makefiles on Unix-like systems and Visual Studio
solution on Windows), \. You can use the following CMake variables to modify the
default behavior:

-   `-DCMAKE_BUILD_TYPE` -- specifies the build mode. Valid values are Release
    and Debug. See the [CMake documentation for more details.
    (Optional)](https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html)

-   `-DBUILD_SHARED_LIBS` -- specifies the link mode. Valid values are ON for
    dynamic linking and OFF for static linking. See [CMake documentation for
    more details.
    (Optional)](https://cmake.org/cmake/help/latest/variable/BUILD_SHARED_LIBS.html)

-   `-G` -- CMake generator. The generator is the native build system to use
    build the source code. All the valid values are described described in the
    CMake documentation [CMake Generators
    Section.](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html)

For example, to build a example in Debug/Static mode run CMake as follows:

```sh
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=ON .. -G "Visual Studio 15 2017" -A x64
```

### Configuring Connext DDS Installation Path and Architecture

The CMake build infrastructure will try to guess the location of your Connext
DDS installation and the Connext DDS architecture based on the default settings
for your host platform.If you installed Connext DDS in a custom location, you
can use the CONNEXTDDS_DIR variable to indicate the path to your RTI Connext DDS
installation folder. For example:

```sh
cmake -DCONNEXTDDS_DIR=/home/rti/rti_connext_dds-x.y.z ..
```

Also, If you installed libraries for multiple target architecture on your system
(i.e., you installed more than one target rtipkg), you can use the
CONNEXTDDS_ARCH variable to indicate the architecture of the specific libraries
you want to link against. For example:

```sh
cmake -DCONNEXTDDS_ARCH=x64Linux3gcc5.4.0 ..
```

## Requirements

To run this example you will need:

- RTI Connext Professional version 6.1.0 or higher.
- CMake version 3.11 or higher
