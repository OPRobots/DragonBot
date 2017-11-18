#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <ATDelay.h>

ATDelay::ATDelay(int delay)
{
	_lastMillis = 0;
	_delay = delay;
}

ATDelay::ATDelay()
{
	_lastMillis = 0;
	_delay = 0;
}

bool ATDelay::check()
{
	return (millis()-_lastMillis) >= _delay;
}

void ATDelay::reset()
{
	_lastMillis = millis();
}

void ATDelay::setDelay(int delay)
{
	_delay = delay;
	ATDelay::reset();
}