# Example Code: Batching

## Building the Example :wrench:

To build this example, first set the environment variables of RTIConnextDDS
as follows:

On *Linux* and *macOS* systems run:

```sh
source <install_dir>/recource/scripts/rtisetenv_<target>.<bash|tcsh|zsh>
```

On *Windows* systems run:

```sh
<install_dir>\resource\scripts\rtisetenv_<target>.bat
```

Once you have run rtisetenv, run `gradle` to generate the Java's `.class`
and `.jar` files from the `<example_dir>/java` directory. This will also call
`rtiddsgen` for you:

```sh
gradle build
```

If you need to clean all the generated files run:

```sh
gradle clean
```

## Running the Example

Run the following commands in two separate command prompts, one for the
publisher and another one for the subscriber, both from the example directory:

On *UNIX* systems run:

```sh
java -cp build/libs/java.jar:$NDDSHOME/lib/java/nddsjava.jar batch_dataPublisher -d <domain_id> -s <samples_to_send> --turbo
java -cp build/libs/java.jar:$NDDSHOME/lib/java/nddsjava.jar batch_dataSubscriber -d <domain_id> -s <samples_to_receive> --turbo
```

On *Windows* systems run:

```sh
java -cp "build\libs\java.jar";"%NDDSHOME%\lib\java\nddsjava.jar" batch_dataPublisher <domain_id> <turbo_mode> <samples_to_send>
java -cp "build\libs\java.jar";"%NDDSHOME%\lib\java\nddsjava.jar" batch_dataSubscriber <domain_id> <turbo_mode> <sleep_periods>
```

Alternatively, you can use `gradle` to run this example:

```sh
gradle run -PmainClass=Publisher --args="-d <domain_id> -s <samples_to_send> --turbo"
gradle run -PmainClass=Subscriber --args="-d <domain_id> -s <samples_to_receive> --turbo"
```

The applications accept up to three arguments:

1.  The `<domain_id>`. Both applications must use the same domain id in order to
    communicate. The default is 0.

2.  How long the examples should run, measured in samples. The default is
    infinite.

3.  If `<turbo_mode>` will be used or not. By default turbo mode is
not enabled, so manual batching will be used. To activate the *Turbo Mode*, use
the `--turbo` option.

## Gradle Build Infrastructure

The `build.gradle` script that builds this example uses a generic plugin called
`com.github.rticommunity.connext-dds-compile-example` that defines all the
necessary constructs to:

1.  Run RTI Code Generator to generate the serialization/deserialization code
    for the types defined in the IDL file associated with the example.

2.  Build the corresponding Publisher and Subscriber applications.

3.  Generate the `.jar` and configure the Class-Path in the MANIFEST.

You will find the definition of the plugin, along with detailed
documentation, in `../../../../resources/gradle_plugin`.
