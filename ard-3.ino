// C++ code
// 3rd Arduino (2nd slave)
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Wire.h>

#define LCD_PIN_RS 8
#define LCD_PIN_EN 9
#define LCD_PIN_DB4 10
#define LCD_PIN_DB5 11
#define LCD_PIN_DB6 12
#define LCD_PIN_DB7 13
#define LCD_PIN_V0 6
#define KEYPAD_R1 A3
#define KEYPAD_R2 A2
#define KEYPAD_R3 A1
#define KEYPAD_R4 A0
#define KEYPAD_C1 5
#define KEYPAD_C2 4
#define KEYPAD_C3 3
#define KEYPAD_C4 2
#define KEYPAD_ROW_COUNT 4
#define KEYPAD_COL_COUNT 4
#define PASSWORD_LENGTH 4

enum States {
	// currently taking input from user for new password
	INPUTTING,
	// sending new password to master
	SENDING,
	// waiting for signal from user
	WAIT,
};

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

States state = WAIT;

String inputted = "";

// first byte is boolean (0 or 1) where 1 means new exists
// the rest is the new passwords
byte send_data[1 + PASSWORD_LENGTH] = {};

void request_ev() {
	Wire.write(send_data, 1 + PASSWORD_LENGTH);
	if (send_data[0]) {
		send_data[0] = false;
		start_wait();
	}
}

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(LCD_PIN_RS, OUTPUT);
	pinMode(LCD_PIN_EN, OUTPUT);
	pinMode(LCD_PIN_DB4, OUTPUT);
	pinMode(LCD_PIN_DB5, OUTPUT);
	pinMode(LCD_PIN_DB6, OUTPUT);
	pinMode(LCD_PIN_DB7, OUTPUT);
	pinMode(LCD_PIN_V0, OUTPUT);

	pinMode(KEYPAD_R1, INPUT);
	pinMode(KEYPAD_R2, INPUT);
	pinMode(KEYPAD_R3, INPUT);
	pinMode(KEYPAD_R4, INPUT);
	pinMode(KEYPAD_C1, INPUT);
	pinMode(KEYPAD_C2, INPUT);
	pinMode(KEYPAD_C3, INPUT);

	send_data[0] = false;

	Serial.begin(9600);

	Wire.begin(3);
	Wire.onRequest(request_ev);

	// write "boot screen" to LCD
	analogWrite(LCD_PIN_V0, 0);
	lcd.begin(16, 2);
	lcd.print("Josep 13519164");
	lcd.setCursor(0, 1);
	lcd.print("Tugas Besar 3");

	delay(250);

	start_wait();
}

void loop()
{
	char key = keypad.getKey();
	// check for inputted key
	if (state != SENDING && key) {
		if (state == WAIT && key == 'A') {
			start_inputting();
		} else {
			switch (key) {
			case 'B':
				start_wait();
				break;
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
				lcd.setCursor(0, 1);
				for (byte _ = 0; _ < 16; ++_) {
					lcd.print(" ");
				}
				lcd.setCursor(0, 1);
			default:
				if ('0' <= key && key <= '9') {
					inputted += key;
					lcd.print(key);

					if (inputted.length() == PASSWORD_LENGTH)
						start_sending();

				}
				break;
			}
		}
	}

	delay(100);
}

void start_sending()
{
	state = SENDING;

	lcd.clear();
	lcd.print("SENDING PASSWD");
	lcd.setCursor(0, 1);
	lcd.print("TO MASTER...");

	send_data[0] = true;
	for (int i = 1; i <= PASSWORD_LENGTH; ++i)
		send_data[i] = inputted[i - 1];

}

void start_wait()
{
	state = WAIT;

	inputted = "";

	lcd.clear();
	lcd.print("PRESS A TO");
	lcd.setCursor(0, 1);
	lcd.print("CHANGE PASSWORD");
}

void start_inputting()
{
	state = INPUTTING;

	lcd.clear();
	lcd.print("ENTER PASSWD:");
	lcd.setCursor(0, 1);
}
