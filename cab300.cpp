/*
 * cab300.cpp - Handles interfacing with CAB300 sensor
 *
Copyright (c) 2015 Collin Kidder

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include "cab300.h"

extern EEPROMSettings settings;

void CAB300::processFrame(CAN_FRAME &frame)
{
	static int32_t lastMillis;
	static uint32_t currentMillis;
	if (frame.id == settings.cab300Address)
	{
		if (frame.data.byte[4] & 1) //ERROR!
		{						
			byte faultCode = frame.data.byte[4] >> 1;
			switch (faultCode)
			{
			case 0x41:
				Logger::error("CAB300 - Error on dataflash CRC");				
				break;
			case 0x42:
				Logger::error("CAB300 - Fluxgate running high freq");
				break;
			case 0x43:
				Logger::error("CAB300 - Fluxgate not oscillating");
				break;
			case 0x44:
				Logger::error("CAB300 - CAB entered failsafe mode");
				break;
			case 0x46:
				Logger::error("CAB300 - Signal not avail");
				break;
			case 0x47:
				Logger::error("CAB300 - Bridge voltage protection");
				break;
			default:
				Logger::error("CAB300 - Something bad happened");
				break;
			}
		}
		else
		{
			int64_t tempCurr;
			lastMillis = currentMillis;
			if (lastMillis == 0) lastMillis = millis() - 1;
			currentMillis = millis();
			tempCurr = frame.data.byte[0] << 24;
			tempCurr += frame.data.byte[1] << 16;
			tempCurr += frame.data.byte[2] << 8;
			tempCurr += frame.data.byte[3];
			tempCurr -= (int64_t)0x80000000;
			amperageReading = (int32_t)(tempCurr);
			float currentValue = amperageReading / 1000.0f;
			//Logger::debug("CAB300 - Current %f", currentValue);
			Logger::debug("CAB300 - Curr AH %i", settings.currentPackAH);
			if (lastMillis > 0)
			{			
			    //currentReading is in milliamps but currentPackAH is in tenths of a microamp so * 10,000 but our time interval
				//is milliseconds and there are 3,600,000 ms per hour so together that's just / 360
				//times the number of milliseconds elapsed.
				//this is subtracted from the current number of AH left in the pack.
				//Which means that negative current really does charge.

				int32_t deltaAH;
				//if (amperageReading > 100) {
					deltaAH = (amperageReading * (int32_t)(currentMillis - lastMillis)) / 360;
				//}
				//else deltaAH = 0;
				Logger::debug("CAB300 - ar: %l, delta = %l", amperageReading, deltaAH);

				if ((deltaAH < 0) || (deltaAH <= settings.currentPackAH))
				{
					int32_t temp = settings.currentPackAH;
					temp -= deltaAH;
					settings.currentPackAH = temp;
				}
				else 
				{
					settings.currentPackAH = 0;
					//Logger::error("PACK TOTALLY EMPTY! QUIT DISCHARGING DAMN IT!");
				}
				if (settings.currentPackAH > settings.maxPackAH) {
					settings.currentPackAH = settings.maxPackAH; //cap at top of capacity
					//Logger::info("Pack is likely fully charged. Might want to stop charging");
				}
			   
			}			
		}
	}
}

//just returns the current amperage reading from the sensor
//could do some smoothing if needed but not done right now.
int32_t CAB300::getAmps()
{
	return amperageReading;
}
