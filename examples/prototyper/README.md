# Prototyper

RTI Prototyper is a tool to accelerate RTI Connext DDS application development
and scenario testing. It provides RTI Connext DDS application developers with a
quick and easy-to-use mechanism to try out realistic scenarios on their own
computer systems and networks, and get immediate information on the expected
performance, resource usage, and behavior of their system.

Starting with version 5.1.0, Prototyper includes an embedded Lua scripting
language engine. Lua is a powerful, fast, lightweight, scripting language that
combines simple procedural syntax with powerful data description constructs
based on associative arrays and extensible semantics. To learn more about Lua,
visit www.lua.org.

The Lua interpreter allows developers to prototype complex application
behaviors without recompiling applications. This allows for rapid development
of test functionality, including sending variable rates of data, data that is
only sent based on events, or other scenarios that cannot be modeled with
simple periodic data.

By embedding a Lua interpreter, Prototyper provides an easy and powerful way to
define the data and behavior of distributed application components. The
integration is seamless. A Lua script implementing the desired behavior can be
embedded directly in the XML or stored in an external file that is loaded at
run time.

In this directory you will find some examples about Prototyper.
For more info check out the
[documentation](https://community.rti.com/static/documentation/connext-dds/6.0.0/doc/manuals/connext_dds/prototyper/RTI_ConnextDDS_CoreLibraries_Prototyper_GettingStarted.pdf).
