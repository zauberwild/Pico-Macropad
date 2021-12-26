/** DISPLAY
 *
 * this file drives the OLED-display
 *
 * created by zauberwild on 16.08.2021
 *
 */

#ifndef display_h
#define display_h
#include "Arduino.h"
#include "config.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "flavortext.h"

//* DEFINING CONSTANTS
#define SCREEN_WIDTH 128	// OLED display width, in pixels
#define SCREEN_HEIGHT 64	// OLED display height, in pixels
#define OLED_RESET 28		// Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

namespace Display
{

	//* variables and objects
	Adafruit_SSD1306 display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

	bool prev_enabled;
	int prev_app;

	//* functions
	/** @brief calculates the x-coordinate of char array on the display
	 * @param text the text to calculate
	 * @param middle the middle point of the text (x-axis)
	 */
	int get_x_coordinate(char *text, int middle)
	{

		int number_of_chars = 0; // count the number of characters
		for (int i = 0; i < 24; i++)
		{
			if (text[i] == 0)
				break;

			number_of_chars++;
		}

		int length = number_of_chars * 6; // calculate the length of the text with the width of one character

		return middle - (length / 2); // return the most left coordinate
	}

	/** @brief start the display
	 */
	void begin()
	{

		// start display and test if it works
		if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
		{
			// Serial.println("[display::begin] DISPLAY NO NO WORKING");
			// Serial.println(F("SSD1306 allocation failed"));
			while (true)
			{
			} // Don't proceed, loop forever
		}

		// Serial.println("[display::begin] display ready to go");

		display.clearDisplay();
		display.display();

		// manage settings for displaying text
		display.setTextColor(1, 0);
		display.setTextSize(1);
	}

	/** @brief a lil startup animation. it will print out random status lines like a linux booting up
	 */
	char buffer[8][24];
	void startupAnim()
	{
		// clear display and reset cursor
		display.clearDisplay();
		display.setCursor(0, 0);

		// pushing every element of the buffer to the left (index downwards, on the display upwards)
		for (int i = 0; i < 7; i++)
		{
			for (int j = 0; j < 24; j++)
			{
				buffer[i][j] = buffer[i + 1][j];
			}
		}

		// generate new line in last index of buffer
		bool constructive = random(100) != 0;		// make deconstructive line sometimes
		getFullLine(&buffer[7][0], constructive);

		// print the text
		for (int i = 0; i < 8; i++)
		{
			display.println(buffer[i]);
		}

		display.display();
	}

	/** @brief update the display text if necessary
	 * @param cur_app the current app to display
	 * @param enabled whether the display should be on or off right now
	 */
	void update(int cur_app, bool enabled)
	{

		//* check if display needs to be activated and/or updated
		if ((enabled == true && prev_enabled == false) || (cur_app != prev_app))
		{

			//NOTE: for placement refer to notes 1-A and 1-B (both version III)
			// first, store data locally
			Config::App app = Config::apps[cur_app];

			display.clearDisplay();

			// draw app number
			display.setCursor(0, 2);
			if (cur_app < 10)
				display.print("0");
			display.print(cur_app);
			// draw title
			display.setCursor(get_x_coordinate(app.name, 64), 2);
			display.print(app.name);
			// draw speaker symbol
			display.drawFastVLine(117, 4, 2, 1);
			display.drawFastVLine(118, 4, 2, 1);
			display.drawFastVLine(119, 3, 4, 1);
			display.drawFastVLine(120, 2, 6, 1);
			display.drawFastVLine(121, 1, 8, 1);
			display.drawFastVLine(123, 4, 2, 1);
			display.drawPixel(124, 2, 1);
			display.drawPixel(124, 7, 1);
			display.drawFastVLine(125, 3, 4, 1);
			display.drawPixel(126, 1, 1);
			display.drawPixel(126, 8, 1);
			display.drawFastVLine(127, 2, 6, 1);

			// draw seperating line
			display.drawFastHLine(0, 14, 128, 1);

			// draw button names
			for (int i = 0; i < 4; i++)
			{ // i and j represent y and x in the button array of the trellis
				for (int j = 0; j < 4; j++)
				{

					//NOTE: for placement refer to notes 1-A and 1-B (both version III)
					char *name = app.buttons[i * 4 + j].name;
					int x = 32 * j + 16;		   // calculate middle x coordinate
					x = get_x_coordinate(name, x); // adjust to left side of the text
					int y = 12 * i + 18;		   // calculate the upper-left corner / y-coordinate

					display.setCursor(x, y);
					display.print(name);
				}
			}

			display.display();
		}
		//* check if display needs to be turned off
		else if (enabled == false && prev_enabled == true)
		{

			display.clearDisplay();
			display.display();
		}

		prev_app = cur_app;
		prev_enabled = enabled;
	}

}

#endif
