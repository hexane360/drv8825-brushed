#include "Arduino.h"
#include "Drv8825.h"

//enable, step, sleep, reset
//dir should be disconnected (internal pulldown)
//M0 should be pulled high (2x microstepping)

Drv8825 driver1 = DRV8825(45,47,48,50);

const int IN1 = 60; //A6
const int IN2 = 61; //A7

unsigned long time1 = 0;
unsigned long time2 = 0;

bool oldIn1 = false;
bool oldIn2 = false;
bool on1 = false;
bool on2 = false;

void setup() {
	Serial.begin(115200);
	Serial.println("Reset");
	randomSeed(analogRead(0));
	
	pinMode(IN1, INPUT_PULLUP);
	pinMode(IN2, INPUT_PULLUP);

	driver1.begin();
	driver1.dir(Dir::Stop, Dir::Stop);
	driver1.run(200);
}

void loop() {
	bool in1 = !digitalRead(IN1);
	bool in2 = !digitalRead(IN2);
	long time = millis();
	if (in1 && !oldIn1 && time-time2 > 20) { //rising edge + debounce
		Serial.println("Starting 1");
		driver1.dir(1, Dir::CW);
		on1 = true;
		time1 = time;
	}
	if (on1 && time-time1 > 10000) {
		Serial.println("Stopping 1");
		driver1.dir(1, Dir::Stop);
		on1 = false;
	}
	if (in2 && !oldIn2 && time-time2 > 20) {
		Serial.println("Starting 2");
		driver1.dir(0, Dir::CW);
		on2 = true;
		time2 = time;
	}
	if (on2 && time-time2 > 10000) {
		Serial.println("Stopping 2");
		driver1.dir(0, Dir::Stop);
		on2 = false;
	}
	oldIn1 = in1;
	oldIn2 = in2;
}
