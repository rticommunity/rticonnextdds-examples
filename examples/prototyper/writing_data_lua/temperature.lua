--*****************************************************************************
--*    (c) 2005-2013 Copyright, Real-Time Innovations, All rights reserved.   *
--*                                                                           *
--*         Permission to modify and use for internal purposes granted.       *
--* This software is provided 'as is', without warranty, express or implied.  *
--*                                                                           *
--*****************************************************************************


--
-- A script that increments and decrements a "Temperature" value

--
-- Interface of the component: outputs of Temperature
-- 
-- The Temperature is written periodically.  To support this, the QoS in the 
-- .xml file is BEST_EFFORT.
--
-- In this case, we have a simple system where the RTI Connext DDS Topics
-- are known in advance and unchanging.  Because of this, we can access 
-- the DataWriters explicitly by name.  To decouple the Topic from the 
-- data, you can use the position of the DataWriter in the queue to look 
-- it up, instead of knowing the Topic name in advance.


local TemperatureWriter = CONTAINER.WRITER['Publisher::TemperatureWriter']


-- Global values:
-- We have global variables, to support this use case.  Global
-- variables are saved across invocations of the script, so they maintain their
-- values every time this is run.
if not TEMPERATURE then 
	TEMPERATURE = {['previous'] = 0, ['low'] = 32, ['high'] = 100} 
	APPID = 234 
	INCREMENT = 1
end

-- The current temperature is a local value
local current 

-- If the temperature hits 120 degrees, start to decrement	
if (TEMPERATURE['previous'] >= 120) then
	INCREMENT = -1
end

-- If the temperature hits 0 degrees, start to increment	
if (TEMPERATURE['previous'] <= 0) then 
	INCREMENT = 1
end

-- Temperature is either increasing or decreasing depending on whether this
-- has crossed a threshhold
current = TEMPERATURE['previous'] + INCREMENT

-- Set up the values for the Temperature and Alarm DataWriters
TemperatureWriter.instance['value'] = current
TemperatureWriter.instance['appID'] = APPID

-- Write the temperature value
TemperatureWriter:write()


-- Set the global previous value of temperature
TEMPERATURE['previous']  = current
