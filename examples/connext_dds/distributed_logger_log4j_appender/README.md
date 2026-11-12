# Example Code: Log4j Appender for RTI Distributed Logger

## Concept

RTI Distributed Logger publishes application log messages over DDS so they can
be monitored remotely. `DDSAppender` connects Log4j 2 to Distributed Logger,
allowing Java applications to send their existing Log4j events over DDS.

## Example Description

The appender uses the Log4j layout to format each message and the logger name
as its DDS category. It maps Log4j levels to Distributed Logger levels and
forwards events through the Distributed Logger Java API. Log4j and Distributed
Logger filtering both apply.

This directory provides reference source and a sample configuration that you
can copy, build, and integrate into your application. It does not include a
ready-to-use example application or build setup. Use your own build mechanism
and supply the Log4j JARs and a Connext installation with the required Java
JARs and native libraries.
