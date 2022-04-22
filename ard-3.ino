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

enum States {
	// currently taking input from user for new password
	INPUTTING,
	// ready to change and waiting for signal from controller
	READY,
	// waiting for signal from user
	WAIT
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

void receive_ev(int n) {
}

void request_ev() {
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

	Serial.begin(9600);

	Wire.begin(3);
	Wire.onReceive(receive_ev);
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
	if (key) {
		switch (key) {
			lcd.print(key);
		}
	}

	delay(100);
}

void start_wait()
{
	state = WAIT;

	lcd.clear();
	lcd.print("PRESS A TO");
	lcd.setCursor(0, 1);
	lcd.print("CHANGE PASSWORD");
}
