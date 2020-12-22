# Example Code: Use the Logging API and explanation of NDDS_Config_LogPrintFormat

Logging is the most basic and important debuggability feature. It allows you to
control how much debugging information is reported during runtime.

## Concept

-   **NDDS_Config_LogVerbosity**: The verbosities at which RTI Connext diagnostic
information is logged.
-   **NDDS_Config_LogLevel**: Level category assigned to RTI Connext log messages
returned to an output device.
-   **NDDS_Config_LogPrintFormat**: The format used to output RTI Connext diagnostic
information.
-   **NDDS_CONFIG_LOG_PRINT_FORMAT_MAXIMAL**: Print all available fields: timestamp,
    thread Name, Activity Context, Module, File and Line, Method, Message Id and
    Backtrace.

## Example Description

In this example we illustrate how to use the Logging API. As in other examples
a DomainParticipant, Type, Topic, Publisher and DataWriter are created.

The verbosity is set to **NDDS_CONFIG_LOG_VERBOSITY_WARNING**, it means that all
warnings and erros will be filter in and will be logged in the terminal.

The **NDDS_CONFIG_LOG_PRINT_FORMAT_MAXIMAL** is set in the *NDDS_Config_LogLevel*
**NDDS_CONFIG_LOG_LEVEL_WARNING**, it means that all the warnings message will
be composed wit all the available RTI Connext diagnostic fields.

Two samples are written and then for the second one we force the write operation
to fail: when using *DDS_BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS*, if source
timestamp is older than the one from a previous write, a warning  will be logged.

```plaintext
    Backtrace:
    #4    /home/rti_connext_dds-6.1.0/lib/x64Linux3gcc5.4.0/libnddscored.so(WriterHistoryMemoryPlugin_addSample+0x68f) [0x7f752ff1f00d]
    #5    /home/rti_connext_dds-6.1.0/lib/x64Linux3gcc5.4.0/libnddscored.so(PRESWriterHistoryDriver_addWrite+0xca7) [0x7f7530239e34]
    #6    /home/rti_connext_dds-6.1.0/lib/x64Linux3gcc5.4.0/libnddscored.so(PRESPsWriter_writeInternal+0x2222) [0x7f75300e112e]
    #7    /home/rti_connext_dds-6.1.0/lib/x64Linux3gcc5.4.0/libnddscd.so(DDS_DataWriter_write_w_timestamp_untyped_generalI+0x9a0) [0x7f7530ab1a35]
    #08    /lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0xf0) [0x7f752f6ba840]
    #09    ./logging_publisher() [0x403039]
[2020-11-27 13:51:52.503762] U00007f753124c700_logging_publish [0x01016FA9,0xB965C9C1,0x4E59B461:0x80000003{E=DW,T=Example logging,C=logging,D=0}|WRITE]
  Mx16:/home/writer_history.1.0/srcC/memory/Memory.c:7411:WriterHistoryMemoryPlugin_addSample:RTI0x2161002:out of order
    Backtrace:
    #4    /home/rti_connext_dds-6.1.0/lib/x64Linux3gcc5.4.0/libnddscored.so(PRESWriterHistoryDriver_getAndLogCollatorFailReason+0xc9) [0x7f753022df79]
    #5    /home/rti_connext_dds-6.1.0/lib/x64Linux3gcc5.4.0/libnddscored.so(PRESWriterHistoryDriver_addWrite+0xd5e) [0x7f7530239eeb]
    #6    /home/rti_connext_dds-6.1.0/lib/x64Linux3gcc5.4.0/libnddscored.so(PRESPsWriter_writeInternal+0x2222) [0x7f75300e112e]
    #7    /home/rti_connext_dds-6.1.0/lib/x64Linux3gcc5.4.0/libnddscd.so(DDS_DataWriter_write_w_timestamp_untyped_generalI+0x9a0) [0x7f7530ab1a35]
    #08    /lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0xf0) [0x7f752f6ba840]
    #09    ./logging_publisher() [0x403039]
[2020-11-27 13:51:52.504785] U00007f753124c700_logging_publish [0x01016FA9,0xB965C9C1,0x4E59B461:0x80000003{E=DW,T=Example logging,C=logging,D=0}|WRITE]
  Mx0D:/home/pres.1.0/srcC/writerHistoryDriver/WriterHistoryDriver.c:782:PRESWriterHistoryDriver_addWrite:RTI0x20d9006:!timestamp order in topic 'Example logging'
[2020-11-27 13:51:52.504805] U00007f753124c700_logging_publish [0x01016FA9,0xB965C9C1,0x4E59B461:0x80000003{E=DW,T=Example logging,C=logging,D=0}|WRITE]
  Mx0D:/home/pres.1.0/srcC/psService/PsReaderWriter.c:3850:PRESPsWriter_writeInternal:RTI0x2000008:!timestamp order
```

For more information about the APIs, please refer to the API Reference HTML
documentation or the [User Manuals](https://community.rti.com/static/documentation/connext-dds/6.1.0/doc/manuals/connext_dds_professional/users_manual/index.htm#users_manual/Controlling_Messages_from_.htm).

## Goal of the Example

The goal of this example is to understand the different fields of
**NDDS_Config_LogPrintFormat**. Reviewing the first warning:

-   **Backtrace**: A list of the function calls that are currently active in
  a thread.

```plaintext
    Backtrace:
    #4    /home/rti_connext_dds-6.1.0/lib/x64Linux3gcc5.4.0/libnddscored.so(WriterHistoryMemoryPlugin_addSample+0x68f) [0x7f752ff1f00d]
    #5    /home/rti_connext_dds-6.1.0/lib/x64Linux3gcc5.4.0/libnddscored.so(PRESWriterHistoryDriver_addWrite+0xca7) [0x7f7530239e34]
    #6    /home/rti_connext_dds-6.1.0/lib/x64Linux3gcc5.4.0/libnddscored.so(PRESPsWriter_writeInternal+0x2222) [0x7f75300e112e]
    #7    /home/rti_connext_dds-6.1.0/lib/x64Linux3gcc5.4.0/libnddscd.so(DDS_DataWriter_write_w_timestamp_untyped_generalI+0x9a0) [0x7f7530ab1a35]
    #08    /lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0xf0) [0x7f752f6ba840]
    #09    ./logging_publisher() [0x403039]
```

- **Timestamp**: The time when the message was logged.

```plaintext
[2020-11-27 13:51:52.503762]
```

-   **Thread Name**: Thread identification strings uniquely identify active threads
when a message is output to the console. A thread may be a user (application)
thread or one of several types of internal threads.

```plaintext
U00007f753124c700_logging_publish
```

-   **Activity Context**: Activity Context provides more context about a logging
message. It is a group of resources and activities associated with an action.
In the following example, the context is "a DataWriter is writing a sample".
    -   Resource: the DataWriter writing the sample. The attributes of the DataWriter
    will be guid, name, kind, topic, type and the domain id.
    -   Activity will be “write a sample”.

```plaintext
[0x01016FA9,0xB965C9C1,0x4E59B461:0x80000003{E=DW,T=Example logging,C=logging,D=0}|WRITE]
```

- **Module**: Id representing the module

```plaintext
Mx16
```

- **File and Line**

```plaintext
writer_history.1.0/srcC/memory/Memory.c:7411
```

- **Method** name where the message is logged

```plaintext
WriterHistoryMemoryPlugin_addSample:RTI0x2161002:out of order
```

- **Message Id**: It is a identification of the message.

```plaintext
RTI0x2161002
```
