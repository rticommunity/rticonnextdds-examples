# Example Code: Log4j Appender for RTI Distributed Logger

See the example description in the top-level [README](../README.md) file.

## Building the Example :wrench:

[DDSAppender.java](DDSAppender.java) forwards Log4j 2 events to RTI Distributed
Logger through its public Java API. This is reference code to integrate into
your application, not a standalone example or build project.

1. Add the source to your application and provide:
   - Matching Log4j API and Core JARs. The appender targets Log4j 2.25.5.
   - Connext's `nddsjava.jar`, Distributed Logger implementation JAR, and
     matching native libraries from your Connext installation.
   - A JDK supported by both products.
2. Put the JARs on the classpath and Connext native libraries on Java's native
   library search path. Setting `NDDSHOME` alone is not sufficient.
3. Enable Log4j's
   [PluginProcessor](https://logging.apache.org/log4j/2.x/manual/plugins.html)
   annotation processor and package the generated plugin metadata.
4. Ensure no RTI JAR on the classpath contains a competing
   `com.rti.dl.adapters.DDSAppender` (previously included with RTI Distributed
   Logger JAR). The legacy constructor and setter API is not preserved.

## Running the Example

### Configuration

Start with [log4j2.properties](log4j2.properties). It sends INFO and higher
events to DDS domain 0 with application identity `Log4jReference`. Attach the
appender alongside your existing appenders as needed.

The main settings use the prefix `appender.dds.`:

| Setting | Default | Purpose |
| --- | --- | --- |
| `type` | Required | Must be `DDSAppender`. |
| `name` | Required | Name referenced by the logger. Cannot be empty. |
| `configureDistLogger` | `true` | Initialize Distributed Logger; use `false` for application-managed initialization. |
| `domainId` | `0` | DDS domain ID, zero or greater. |
| `applicationKind` | Empty | Application identity, up to 255 characters. |
| `qosLibrary`, `qosProfile` | Unset | If using custom QoS, provide both names and load the QoS before initialization. Neither name can be empty. |
| `filterLevel` | `INFO` | Initial Distributed Logger filter. |
| `ignoreExceptions` | `true` | Report forwarding errors through Log4j's error handler. |
| `layout` | `PatternLayout` | Thread-safe `StringLayout`; binary layouts are unsupported. |

Standard Log4j appender filters are also supported. The logger name becomes
the DDS category; layout output, including whitespace and exceptions, is
forwarded unchanged.

Log4j filters messages before sending them to the appender. Distributed Logger
then filters them before forwarding over DDS. To forward DEBUG messages, allow
them at both stages:

```properties
rootLogger.level = DEBUG
appender.dds.filterLevel = DEBUG
```

If either setting remains at `INFO`, DEBUG messages are discarded. To also
forward TRACE messages, set both to `TRACE`. When using application-managed
initialization, configure the Distributed Logger filter in your application
instead of using `appender.dds.filterLevel`.

### Initialization

All DDS appenders share the Distributed Logger singleton.

- **Appender-managed initialization:** With `configureDistLogger=true`, the
  appender must start before any other code creates the singleton. Otherwise,
  configuration fails and the appender remains unstarted.
- **Application-managed initialization:** For multiple appenders or Log4j
  reconfiguration, configure `Options`, check that `DistLogger.setOptions()`
  returns `true`, then call `DistLogger.getInstance()` before obtaining Log4j
  loggers, including static logger fields. Set `configureDistLogger=false` on
  every appender; their domain, identity, QoS, and filter settings are validated
  but not applied. Without application initialization, default options may be
  used. Configure additional options or an application-owned participant here.

### Shutdown

Keep `shutdownHook = disable` in the Log4j configuration and shut down in order:

1. Stop and join log-producing threads and prevent further logging.
2. Call `LogManager.shutdown()` to stop Log4j and its asynchronous work.
3. Call `DistLogger.delete()` exactly once for the initialized singleton.
4. Tear down application-owned DDS entities and participants. If your
   application owns the DDS runtime, call
   `DomainParticipantFactory.finalize_instance()` last.

Stopping an appender does not delete Distributed Logger. Coordinate all users
of the singleton; deletion is irreversible, so do not log or restart it afterward.

## Limitations

- DDS string bounds are 512 for the category and 4096 for the message.
  Oversized text is not truncated; account for encoding and stack traces.
