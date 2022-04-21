// C++ code
// 2nd arduino (2nd worker)

#include <Wire.h>

#define PIR_ENTER_PIN 12
#define PIR_OUT_PIN 11
#define BUTTON_PIN 7

unsigned short people_counter = 0;
bool door_open = false;

void receive_ev(int how_many)
{
	door_open = (Wire.read() == 1);
}

void request_ev()
{
	Wire.write(door_open);
}

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);

	pinMode(PIR_ENTER_PIN, INPUT);
	pinMode(PIR_OUT_PIN, INPUT);
	pinMode(BUTTON_PIN, INPUT);

	Serial.begin(9600);

	Wire.begin(2);
	Wire.onReceive(receive_ev);
	Wire.onRequest(request_ev);
}

void loop()
{
	byte pir_enter_reading = digitalRead(PIR_ENTER_PIN);
	byte pir_out_reading = digitalRead(PIR_OUT_PIN);
	byte button_reading = digitalRead(BUTTON_PIN);

	// if button is pressed, send signal to arduino 1
	if (button_reading == LOW) {
		door_open = true;
	}

	// update people counter when the door is open
	if (door_open) {
		// delay is needed to avoid counting the same person twice
		if (pir_enter_reading == HIGH && pir_out_reading == LOW) {
			// someone entered the room
			people_counter++;
		} else if (pir_enter_reading == LOW && pir_out_reading == HIGH) {
			// someone left the room
			people_counter = people_counter > 0 ? people_counter - 1 : 0;
		}

		if (pir_enter_reading == HIGH || pir_out_reading == HIGH) {
			// if someone entered or left, the door should be closed
			door_open = false;
		}
	}

	// turn on  light if people are inside
	digitalWrite(LED_BUILTIN, people_counter > 0);
	

	delay(100);
}
