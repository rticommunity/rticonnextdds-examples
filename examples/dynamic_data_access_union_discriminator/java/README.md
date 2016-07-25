# Example Code: Access Union Discriminator In Dynamic Data

## Building Java Example
The example is contained in the *UnionExample.java* file. Before compiling or
running the example, make sure the environment variable `NDDSHOME` is set to the
directory where your version of *RTI Connext* is installed.

Before compiling in Java, make sure that the desired version of the *javac*
compiler is in your `PATH` environment variable.

* On *Windows* systems run:
```
javac -classpath .;%NDDSHOME%\lib\java\nddsjava.jar UnionExample.java
```

* On *UNIX* systems (including *Linux* and *MacOS X*) run:
```
javac -classpath .:$NDDSHOME/lib/java/nddsjava.jar UnionExample.java
```

## Running Java Example
Run the following command from the example directory to execute the application.

* On *Windows* Systems:
```
java -cp .;%NDDSHOME%\lib\java\nddsjava.jar UnionExample
```

* On *UNIX* systems:
```
java -cp .:$NDDSHOME/lib/java/nddsjava.jar UnionExample
```
