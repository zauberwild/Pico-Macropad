/** LED-CONTROLLER
 * 
 * this file keeps track of the time and lets the leds automatically dim
 * 
 * created by zauberwild on 16.08.2021
 * 
 */

#ifndef led_controller_h
#define led_controller_h
#include "Arduino.h"

//* DEFINING CONSTANTS
// timing (refer to notes page 3/A)
#define AWAKE_TIME 5000
#define DIMMING_TIME 1000
#define STANDBY_TIME 25000
#define DISABLING_TIME 500
// leds settings (everything in %)
#define FULL_POWER 25	   // the brightest the led will shine in awake mode
#define STANDBY_POWER 7	   // the percentage of how much the led should shine when in standby
#define FULL_POWER_ROT 100 // same, but only for the rotary encoders
#define STANDBY_POWER_ROT 40

namespace LedController
{

	//* variables
	unsigned long last_update = 0; // saves the last last update with millis()
	bool off = false;			   // true when turned off
	bool displayState = false;	   // true if display on, false if display off
	bool rotaryState = false;	   // true if rotary led are on, false if leds are off

	//* functions
	/** @brief resets the timer, turning the leds back on and (if necessary) turing back on
	 * @return true, when still on and action can be taken;
	 * false, when off and a second press is necessary for any action
	 */
	bool resetTimer()
	{
		last_update = millis();

		return (!off);
	}

	/**	@brief calculates the luminance of the leds for the neotrellis based on the time since the last action
	 * @return the luminance as a percentage between 0 (off) and 100 (full power)
	 */
	float updateForNeoTrellis()
	{

		float luminance;
		int time = millis() - last_update; // time since the last update

		//* calculating the luminance and simultaneously setting led and display state
		// refer to notes 3-A
		// disabled
		if (time >= AWAKE_TIME + DIMMING_TIME + STANDBY_TIME + DISABLING_TIME)
		{
			luminance = 0;
			off = true;
		}
		// disabling / turning off
		else if (time >= AWAKE_TIME + DIMMING_TIME + STANDBY_TIME)
		{
			luminance = map(time - (AWAKE_TIME + DIMMING_TIME + STANDBY_TIME), 0, DISABLING_TIME, STANDBY_POWER, 0);
			off = false;
		}
		// standby / leds dimmed & display off
		else if (time >= AWAKE_TIME + DIMMING_TIME)
		{
			luminance = STANDBY_POWER;
			off = false;
		}
		// first dimming phase
		else if (time >= AWAKE_TIME)
		{
			luminance = map(time - AWAKE_TIME, 0, DIMMING_TIME, FULL_POWER, STANDBY_POWER);
			off = false;
		}
		// awake phase
		else
		{
			luminance = FULL_POWER;
			off = false;
		}

		return (luminance);
	}

	/**	@brief calculates the luminance of the leds for the rotary encoders based on the time since the last action. also updates the display state
	 * @return the luminance as a percentage between 0 (off) and 100 (full power)
	 * 
	 */
	float updateForRotaryEncoder()
	{

		float luminance;
		int time = millis() - last_update; // time since the last update

		//* calculating the luminance and simultaneously setting led and display state
		// refer to notes 3-A
		// disabled
		if (time >= AWAKE_TIME + DIMMING_TIME + STANDBY_TIME + DISABLING_TIME)
		{
			luminance = 0;
			off = true;
			displayState = false;
			rotaryState = false;
		}
		// disabling / turning off
		else if (time >= AWAKE_TIME + DIMMING_TIME + STANDBY_TIME)
		{
			luminance = map(time - (AWAKE_TIME + DIMMING_TIME + STANDBY_TIME), 0, DISABLING_TIME, STANDBY_POWER_ROT, 0);
			off = false;
			displayState = false;
			rotaryState = true;
		}
		// standby / leds dimmed & display off
		else if (time >= AWAKE_TIME + DIMMING_TIME)
		{
			luminance = STANDBY_POWER_ROT;
			off = false;
			displayState = false;
			rotaryState = true;
		}
		// first dimming phase
		else if (time >= AWAKE_TIME)
		{
			luminance = map(time - AWAKE_TIME, 0, DIMMING_TIME, FULL_POWER_ROT, STANDBY_POWER_ROT);
			off = false;
			displayState = true;
			rotaryState = true;
		}
		// awake phase
		else
		{
			luminance = FULL_POWER_ROT;
			off = false;
			displayState = true;
			rotaryState = true;
		}

		return (luminance);
	}

	/** @brief returns wether the rotary encoder leds should be on or off
	 * @return true for led on; false for led off
	 */
	bool getRotaryState()
	{
		return (rotaryState);
	}

	/** @brief returns wether the display should be on or off
	 * @return true for display on; false for display off
	 */
	bool getDisplayState()
	{
		return (displayState);
	}

}

#endif