// C++ code
// 1st Arduino (1st worker)
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Servo.h>
#include <Wire.h>
#include <stdio.h>

// pins and other constants
#define LCD_PIN_RS 13
#define LCD_PIN_EN 12
#define LCD_PIN_DB4 10
#define LCD_PIN_DB5 9
#define LCD_PIN_DB6 8
#define LCD_PIN_DB7 7
#define LCD_PIN_V0 11
#define KEYPAD_R1 5
#define KEYPAD_R2 4
#define KEYPAD_R3 3
#define KEYPAD_R4 2
#define KEYPAD_C1 A0
#define KEYPAD_C2 A1
#define KEYPAD_C3 A2
#define KEYPAD_C4 A3
#define KEYPAD_ROW_COUNT 4
#define KEYPAD_COL_COUNT 4
#define DOOR_PIN 6

enum States {
	LOCKED,
	OPENED,
	INPUTTING,
	WAIT,
};

// global variable setup
// \b is ASCII for backspace while * will be used for reset
char keypad_keys[KEYPAD_ROW_COUNT][KEYPAD_COL_COUNT] = {
	{'1', '2',  '3', 'A'},
	{'4', '5',  '6', 'B'},
	{'7', '8',  '9', 'C'},
	{'*', '0', '\b', 'D'},
};
byte keypad_pin_rows[KEYPAD_ROW_COUNT] = {
	KEYPAD_R1,
	KEYPAD_R2,
	KEYPAD_R3,
	KEYPAD_R4,
};
byte keypad_pin_cols[KEYPAD_COL_COUNT] = {
	KEYPAD_C1,
	KEYPAD_C2,
	KEYPAD_C3,
	KEYPAD_C4,
};

Keypad keypad = Keypad(
	makeKeymap(keypad_keys),
	keypad_pin_rows,
	keypad_pin_cols,
	KEYPAD_ROW_COUNT,
	KEYPAD_COL_COUNT
);

LiquidCrystal lcd(
	LCD_PIN_RS,
	LCD_PIN_EN,
	LCD_PIN_DB4,
	LCD_PIN_DB5,
	LCD_PIN_DB6,
	LCD_PIN_DB7
);

Servo door;

String correct_password = "5698";
String inputted = "";

// states
States state = WAIT;

int starting_time = 0;
/// value is 1 <= var <= 99
int max_locked_time = 10;
int max_input_time = 15;

void setup()
{
	// pin modes
	pinMode(LCD_PIN_RS, OUTPUT);
	pinMode(LCD_PIN_EN, OUTPUT);
	pinMode(LCD_PIN_DB4, OUTPUT);
	pinMode(LCD_PIN_DB5, OUTPUT);
	pinMode(LCD_PIN_DB6, OUTPUT);
	pinMode(LCD_PIN_DB7, OUTPUT);
	pinMode(LCD_PIN_V0, OUTPUT);

	pinMode(KEYPAD_R1, INPUTTING);
	pinMode(KEYPAD_R2, INPUTTING);
	pinMode(KEYPAD_R3, INPUTTING);
	pinMode(KEYPAD_R4, INPUTTING);
	pinMode(KEYPAD_C1, INPUTTING);
	pinMode(KEYPAD_C2, INPUTTING);
	pinMode(KEYPAD_C3, INPUTTING);

	pinMode(DOOR_PIN, OUTPUT);

	// set LCD contrast
	analogWrite(LCD_PIN_V0, 0);

	// write "boot screen" to LCD
	lcd.begin(16, 2);
	lcd.print("Josep 13519164");
	lcd.setCursor(0, 1);
	lcd.print("Tugas Besar 3");
	lcd.setCursor(0, 0);

	// set door angle to 0
	door.attach(DOOR_PIN);
	door.write(0);

	Serial.begin(9600);
	Wire.begin();

	delay(250);
	// clear "boot screen"
	lcd.clear();
}

void loop()
{
	if (state == LOCKED || state == OPENED) {
		// if the device is locked, check current time
		int cur_time = millis() / 1000;
		// then, write locked message to first row
		lcd.setCursor(0, 0);
		lcd.print(state == OPENED ? "UNLOCKED" : "LOCKED");
		if (cur_time - starting_time < max_locked_time) {
			// if curent time is smaller than starting time + max_locked_time,
			lcd.setCursor(0, 1);
			// write current time to LCD and then wait for 1 sec
			char s[6];
			sprintf(s, "%02d/%02d", cur_time - starting_time, max_locked_time);
			lcd.print(s);

			Wire.requestFrom(2, 1);
			// check whether need to close the door when state is opened
			if (Wire.available()) {
				bool door_open = Wire.read();
				if (state == OPENED && !door_open)
					end_opened_subroutine();
				else if (state == LOCKED && door_open)
					start_opened_subroutine();

				return;
			}
		} else {
			// else, unlock the device
			lcd.clear();
			// check previous state, if it was opened send signal
			// to slave arduino to change state to closed
			if (state == OPENED)
				end_opened_subroutine();

			state = WAIT;
		}

		delay(100);
		// after both, return from loop()
		return;
	}

	// ask arduino 1 whether the door is
	// opened by the button or not
	Wire.requestFrom(2, 1);
	if (Wire.available()) {
		bool door_open = Wire.read();
		if (door_open) {
			start_opened_subroutine();
			return;
		}
	}

	// at this point, the state is either WAIT or INPUTTING

	if (state == INPUTTING) {
		// if input has already started, count passed seconds
		int sec = millis() / 1000 - starting_time;
		if (sec >= max_input_time) {
			// if passsed seconds is greater than max_input_time, change state to locked
			// 1. set starting time to now
			// 2. set locked to true
			starting_time = millis() / 1000;
			state = LOCKED;
			lcd.clear();
		} else {
			// else, write time to LCD
			// we LCD cursor set to [-5] index
			lcd.setCursor(11, 1);
			// create the string to write
			// 2 digits current time + '/' + 2 digits max time + '\0' (6 chars)
			char s[6];
			sprintf(s, "%02d/%02d", sec, max_input_time);
			// print to LCD
			lcd.print(s);
			// ready LCD to print input
			lcd.setCursor(inputted.length(), 1);
		}
	} else {
		// if input has not started, prompt user to "unlock device"
		// wait for user input
		lcd.setCursor(0, 0);
		lcd.print("PRESS A");
		lcd.setCursor(0, 1);
		lcd.print("ON KEYPAD");
	}

	char key = keypad.getKey();
	// check for inputted key
	if (key) {
		if (state != INPUTTING && key == 'A') {
			// set state to input_started when A is pressed and input has not started
			state = INPUTTING;
			// get starting time
			starting_time = millis() / 1000;
			// clear the LCD
			lcd.clear();
			// then prompt the user to enter password
			lcd_prompt_enter_password();
		} else if (state == INPUTTING) {
			// if state is already input started, check for other possibilities
			switch (key) {
			case '\b':
				// ascii \b is backspace
				// remove the last character from inputted
				inputted.remove(inputted.length() - 1);
				// set the last character written in the LCD to " " (blank)
				lcd.setCursor(inputted.length(), 1);
				lcd.print(" ");
				lcd.setCursor(inputted.length(), 1);
				break;
			case '*':
				// * is used as reset
				inputted = "";
				lcd_clear_row(1);
				break;
			default:
				// only accept if input is digit
				if ('0' <= key && key <= '9') {
					// any other keys
					inputted += key;
					// print * instead of the real key to hide password
					lcd.print('*');

					// check if inputted is correct. Done only when the length
					// inputted is the same as the length of the correct password.
					if (inputted.length() == correct_password.length()) {
						if (inputted == correct_password)
							start_opened_subroutine();
						else
							start_incorrect_passwd_subroutine();
					}
				}
			} // switch (key)
		} // if (!input_started && key == 'A')
	} // if (key)

	// add a bit of delay between loop
	delay(100);
}

/**
 * A procedure to clear row-th row of the LCD (row starts at 0)
 */
void lcd_clear_row(int row)
{
	// iterate 15 to 0 because there are 16 columns
	for (int i = 15; i > -1; --i) {
		lcd.setCursor(i, row);
		lcd.print(" ");
	}
}

/**
 * A procedure to clear the first row of the LCD
 * and then write enter password prompt to LCD.
 * this procedure should be called after the LCD is cleared.
 */
void lcd_prompt_enter_password()
{
	lcd.setCursor(0, 0);
	lcd.print("ENTER PASSWORD:");
	lcd.setCursor(0, 1);
}

/**
 * A procedure to start the unlocked subroutine, which will:
 * 1. reset the inputted string
 * 2. set state to opened
 * 3. clear the LCD then write unlocked message
 * 4. send signal to slave arduino to change state to opened
 * 5. opens the door
 */
void start_opened_subroutine()
{
	// reset state
	inputted = "";
	state = OPENED;

	// set starting time of unlocked
	starting_time = millis() / 1000;

	// print "UNLOCKED" to the LCD
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("UNLOCKED");

	Wire.beginTransmission(2);
	Wire.write(1);
	Wire.endTransmission();

	// open the door for 1 sec
	door.write(90);
}

/**
 * A procedure to start the unlocked subroutine, which will:
 * 1. set state to wait
 * 2. clear the LCD
 * 3. closes the door
 * 4. send signal to slave arduino to change state to opened
 */
void end_opened_subroutine()
{
	state = WAIT;

	lcd.clear();

	door.write(0);

	Wire.beginTransmission(2);
	Wire.write(0);
	Wire.endTransmission();
	lcd.clear();
}

/**
 * A procedure to start the wrong password subroutine, which will:
 * 1. reset the inputted string
 * 2. clear the LCD and then write incorrect password message
 * 3. wait for 1 sec
 * 4. clear the LCD and then write the enter password prompt
 */
void start_incorrect_passwd_subroutine()
{
	// clear user input
	inputted = "";

	// write to the LCD for 0.5 sec
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("INCORRECT");
	lcd.setCursor(0, 1);
	lcd.print("PASSWORD");
	delay(500);

	// clear the LCD and re-write the prompt
	lcd.clear();
	lcd_prompt_enter_password();
}
