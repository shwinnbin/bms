/*
 * CanbusHandler.h - Handles setup and control of canbus link
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

#include <Arduino.h>
#include "SamNonDuePin.h" //Non-supported SAM3X pin library
#include <DueTimer.h>
#include "due_can.h"
#include "Logger.h"
#include "config.h"
#include "i2c_adc.h"
#include "cab300.h"


#ifndef CANBUSCLASS_H_
#define CANBUSCLASS_H_

class CANBusHandler
{
public:
	void canbusTermEnable();
	void canbusTermDisable();
	void setup();
	static CANBusHandler *getInstance();
	void gotFrame(CAN_FRAME *frame);
	void loop();
protected:
private:
	static CANBusHandler* instance;
	ADCClass *adc;
	CAB300 *cab300;
};
#endif