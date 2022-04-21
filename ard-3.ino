// C++ code
// 3rd Arduino (2nd slave)

#include <Keypad.h>
#include <Wire.h>

#define KEYPAD_R1 9
#define KEYPAD_R2 8
#define KEYPAD_R3 7
#define KEYPAD_R4 6
#define KEYPAD_C1 5
#define KEYPAD_C2 4
#define KEYPAD_C3 3
#define KEYPAD_C4 2
#define KEYPAD_ROW_COUNT 4
#define KEYPAD_COL_COUNT 4

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

String inputted = "";

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);

	pinMode(KEYPAD_R1, INPUT);
	pinMode(KEYPAD_R2, INPUT);
	pinMode(KEYPAD_R3, INPUT);
	pinMode(KEYPAD_R4, INPUT);
	pinMode(KEYPAD_C1, INPUT);
	pinMode(KEYPAD_C2, INPUT);
	pinMode(KEYPAD_C3, INPUT);

	Serial.begin(9600);
}

void loop()
{
	char key = keypad.getKey();
	// check for inputted key
	if (key) {
		Serial.println(key);
		if ('0' <= key && key <= '9') {
			digitalWrite(LED_BUILTIN, HIGH);
		} else {
			digitalWrite(LED_BUILTIN, LOW);
		}
	}

	delay(100);
}
