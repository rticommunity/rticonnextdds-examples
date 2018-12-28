====================================
 Example Code -- Writing Data in Lua
====================================

Concept
-------
This example is generating data and writing it over DDS using Lua.  This 
example uses RTI Connext DDS 5.2.0.

This is interesting for two reasons:
1. This shows how to create simple DDS applications for prototyping or
testing systems without generating data types from IDL or compiling code. 
2. This script can be modified as the Prototyper application is running,
and the modifications will be applied immediately as soon as the script
file is saved.

Example Description
-------------------

In this example, the RTI Prototyper with Lua loads the XML configuration and 
a Lua script.  

The XML file defines:
1. A SensorData QoS profile
2. A TemperatureType data type
3. A domain 0 that uses the TemperatureType for a Temperature Topic.
4. A configuration with a DataWriter that writes Temperature data
5. A configuration with a DataReader that reads Temperature data 

The Lua script accesses the DataWriter and data type that are
defined in the XML file.  It then generates data values between 0 and 120, and
uses the DataWriter to write them.

The script stores the previous, max, and min temperatures in a global variable 
called TEMPERATURE.  This is initialized only once when the script is loaded.

It accesses the DataWriter by name, using the following code:
    CONTAINER.WRITER['Publisher::TemperatureWriter']

Then, it increments or decrements the temperature, and updates the DataWriter's
data instance with the latest values to write:
    TemperatureWriter.instance['value'] = current
    TemperatureWriter.instance['appID'] = APPID

Then, it writes the value to the network:
    TemperatureWriter:write()

Run the Example
---------------
In two separate command prompt windows for the sender and receiver. Run
the following commands from the example directory:

 - For the sender run:
Windows: %NDDSHOME%\bin\rtiddsprototyper -cfgFile prototyper_config.xml -luaFile temperature.lua
Linux: $NDDSHOME%\bin\rtiddsprototyper -cfgFile prototyper_config.xml -luaFile temperature.lua

Once you run the sender, you have to type 0 in order to pick a sender.

 - For the receiver run:
Windows: %NDDSHOME%\bin\rtiddsprototyper -cfgFile prototyper_config.xml
Linux: $NDDSHOME%\bin\rtiddsprototyper -cfgFile prototyper_config.xml

Type 1 to create a default receiver (it is not defined in any .lua file). 

When you run both the sender and the receiver, you will see that the temperature
starts at 0, increases to 120, and then decreases to 0.

