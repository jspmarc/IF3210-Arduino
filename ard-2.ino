// C++ code
// 2nd arduino (1st slave)

#define PIR_PIN 12
#define IR_PIN 7

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(PIR_PIN, INPUT);
	Serial.begin(9600);
}

void loop()
{
	int pir_reading = digitalRead(PIR_PIN);
	if (pir_reading == HIGH)
		digitalWrite(LED_BUILTIN, HIGH);
	else
		digitalWrite(LED_BUILTIN, LOW);

	int ir_reading = digitalRead(IR_PIN);

	delay(100);
}
