# Example Code: Access Union Discriminator In Dynamic Data

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

Run the following command from the example directory to execute the
application:

On *UNIX* systems run:

```sh
java -cp build/libs/java.jar:$NDDSHOME/lib/java/nddsjava.jar UnionExample
```

On *Windows* systems run:

```sh
java -cp "build\libs\java.jar";"%NDDSHOME%\lib\java\nddsjava.jar" UnionExample
```

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
