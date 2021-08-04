# Example Code: Deadlines and Content Filtering

## Building the Example :wrench:

To build this example, first set all the environment variables as follows:

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
java -cp build/libs/java.jar:$NDDSHOME/lib/java/nddsjava.jar deadline_contentfilterPublisher <domain_id> <samples_to_send>
java -cp build/libs/java.jar:$NDDSHOME/lib/java/nddsjava.jar deadline_contentfilterSubscriber <domain_id> <sleep_periods>
```

On *Windows* systems run:

```sh
java -cp "build\libs\java.jar";"%NDDSHOME%\lib\java\nddsjava.jar" deadline_contentfilterPublisher <domain_id> <samples_to_send>
java -cp "build\libs\java.jar";"%NDDSHOME%\lib\java\nddsjava.jar" deadline_contentfilterSubscriber <domain_id> <sleep_periods>
```

Alternatively, you can use `gradle` to run this example:

```sh
gradle run -PmainClass=Publisher --args="<domain_id> <samples_to_send>"
gradle run -PmainClass=Subscriber --args="<domain_id> <sleep_periods>"
```

The applications accept up to two arguments:

1.  The `<domain_id>`. Both applications must use the same domain ID in order
to communicate. The default is 0.

2.  How long the examples should run, measured in samples for the publisher
and sleep periods for the subscriber. A value of '0' instructs the application
to run forever; this is the default.

## Gradle Build Infrastructure

The `build.gradle` script that builds this example uses a generic plugin called
`com.github.rticommunity.connext-dds-build-example` that defines all the
necessary constructs to:

1.  Run RTI Code Generator to generate the serialization/deserialization code
for the types defined in the IDL file associated with the example.

2.  Build the corresponding Publisher and Subscriber applications.

3.  Generate the `.jar` and configure the Class-Path in the MANIFEST.

You will find the definition of the plugin, along with detailed
documentation, in `../../../../resources/gradle_plugin`.
