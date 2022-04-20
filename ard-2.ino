// C++ code
// 2nd arduino (1st slave)

#include <Wire.h>

#define PIR_ENTER_PIN 12
#define PIR_OUT_PIN 11
#define BUTTON_PIN 7

unsigned short people_counter = 0;
bool door_open = false;

void receive_ev(int how_many)
{
	Serial.print("Received event: ");
	door_open = (Wire.read() == 1);
	Serial.print(door_open ? "open" : "closed");
}

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);

	pinMode(PIR_ENTER_PIN, INPUT);
	pinMode(PIR_OUT_PIN, INPUT);
	pinMode(BUTTON_PIN, INPUT);

	Serial.begin(9600);

	Wire.begin(1);
	Wire.onReceive(receive_ev);
}

void loop()
{
	byte pir_enter_reading = digitalRead(PIR_ENTER_PIN);
	byte pir_out_reading = digitalRead(PIR_OUT_PIN);

	if (door_open) {
		if (pir_enter_reading == HIGH && pir_out_reading == LOW) {
			people_counter++;
			delay(1500);
		}
		if (pir_enter_reading == LOW && pir_out_reading == HIGH) {
			people_counter = people_counter > 0 ? people_counter - 1 : 0;
			delay(1500);
		}
	}
	digitalWrite(LED_BUILTIN, people_counter > 0);
	

	delay(100);
}
