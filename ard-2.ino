// C++ code
// 2nd arduino (1st slave)

#define PIR_ENTER_PIN 12
#define PIR_OUT_PIN 11
#define BUTTON_PIN 7

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(PIR_ENTER_PIN, INPUT);
	pinMode(PIR_OUT_PIN, INPUT);
	pinMode(BUTTON_PIN, INPUT);
	Serial.begin(9600);
}

unsigned short people_counter = 0;

void loop()
{
	byte pir_enter_reading = digitalRead(PIR_ENTER_PIN);
	byte pir_out_reading = digitalRead(PIR_OUT_PIN);

	if (pir_enter_reading == HIGH && pir_out_reading == LOW) {
		people_counter++;
		delay(1500);
    }
	if (pir_enter_reading == LOW && pir_out_reading == HIGH) {
		people_counter = people_counter > 0 ? people_counter - 1 : 0;
		delay(1500);
    }
	digitalWrite(LED_BUILTIN, people_counter > 0);
	

	delay(100);
}
