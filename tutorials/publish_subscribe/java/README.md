# Tutorial: Publish-Subscribe

This code is part of the Connext Publish-Subscribe tutorial.

## Building the Example :wrench:

You can build the example using Gradle as follows.

1. Setting up the environment for Connext:

Use the following script to configure your shell environment variables to run
Connext executables and load dynamic libraries.

* Linux

If you’re using Bash, run this:

```sh
source <installdir>/resource/scripts/rtisetenv_<architecture>.bash
```

If you’re using the Z shell, run this:

```sh
source <installdir>/resource/scripts/rtisetenv_<architecture>.zsh
```

* macOS

If you’re using Bash, run this:

```sh
source <installdir>/resource/scripts/rtisetenv_<architecture>.bash
```

If you’re using the Z shell, run this:

```sh
source <installdir>/resource/scripts/rtisetenv_<architecture>.zsh
```

If you’re using the tcsh shell, run this:

```sh
source <installdir>/resource/scripts/rtisetenv_<architecture>.tcsh
```

* Windows

```sh
<installdir>\resource\scripts\rtisetenv_<architecture>.bat
```

2. Build the applications:

* Linux or macOs

```sh
./gradlew build
```

* Windows

```sh
./gradlew.bat build
```

## Running the Applications :rocket:

* Linux or macOs

```sh
./gradlew runPublisher
...
```

```sh
./gradlew runSubscriber
...
```

* Windows

```sh
./gradlew.bat runPublisher
...
```

```sh
./gradlew.bat runSubscriber
...
```
