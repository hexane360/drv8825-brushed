#include "Arduino.h"
#include "Drv8825.h"


Drv8825::Drv8825(int enable_pin, int step_pin, int sleep_pin, int reset_pin) : ENABLE(enable_pin), STEP(step_pin), SLEEP(sleep_pin), RESET(reset_pin) {
	pinMode(ENABLE, OUTPUT);
	pinMode(STEP, OUTPUT);
	pinMode(SLEEP, OUTPUT);
	pinMode(RESET, OUTPUT);

	digitalWrite(ENABLE, HIGH); //low to enable
	digitalWrite(STEP, LOW);
	digitalWrite(SLEEP, HIGH); //high to wake
	digitalWrite(RESET, LOW); //low to reset
}

Drv8825::State Drv8825::getState() const { return _state; }

void Drv8825::begin() {
	pinMode(ENABLE, OUTPUT);
	pinMode(STEP, OUTPUT);
	pinMode(SLEEP, OUTPUT);
	pinMode(RESET, OUTPUT);
	
	digitalWrite(STEP, HIGH);
	digitalWrite(ENABLE, HIGH); //low to enable
	digitalWrite(SLEEP, HIGH); //high to wake
	reset();
	//digitalWrite(STEP, HIGH);
}

void Drv8825::sleep() {
	digitalWrite(SLEEP, LOW);
	_state.slept = true;
}

void Drv8825::wake() {
	digitalWrite(SLEEP, HIGH);
	_state.slept = false;
	delayMicroseconds(800);
}

void Drv8825::reset() {
	if (_state.slept) return; //can't reset while asleep
	digitalWrite(RESET, LOW);
	delay(50);
	digitalWrite(RESET, HIGH);
	_state = Drv8825::State();
}

void Drv8825::run(int speed) {
	if (speed < 0 || speed > 255) return;
	if (speed == 0) {
		stop();
		return;
	}
	_state.speed = speed;
	_state.stopped = false;
	if (_state.motor1 || _state.motor2) { //if enabled by dir
		analogWrite(ENABLE, 255-speed);
		_state.enabled = true;
	}
}

//sets one motor direction without affecting the other
void Drv8825::dir(int motor, Dir direction) {
	switch (motor) {
	case 0:
		dir(direction, _state.motor2);
	break;
	case 1:
		dir(_state.motor1, direction);
	break;
	}
}

//sets both motor directions simultaneously
void Drv8825::dir(Dir motor1, Dir motor2) {
	if (_state.slept) return; //can't change direction while asleep
	_state.motor1 = motor1;
	_state.motor2 = motor2;
	int end;
	switch (motor2) {
	case Dir::Stop:
		if (!motor1) {
			_state.enabled = false;
			digitalWrite(ENABLE, HIGH);
			return;
		}
		end = (motor1 == Dir::CW) ? 0 : 4;
	break;
	case Dir::CW:
		end = 2 - motor1;
	break;
	case Dir::CCW:
		end = 6 + motor1;
	break;
	}
	step(end - _state.step + (end < _state.step ? 8 : 0));
	if (!_state.enabled && !_state.stopped) { //enable motor
		_state.enabled = true;
		analogWrite(ENABLE, 255-_state.speed);
	}
}

void Drv8825::step(int num) {
	if (_state.slept) return; //can't step while asleep
	for (int i = 0; i < num; i++) {
		digitalWrite(STEP, LOW);
		delayMicroseconds(20);
		digitalWrite(STEP, HIGH);
		delayMicroseconds(20);
	}
	_state.step = (_state.step + num)%8;
}

void Drv8825::stop() {
	_state.stopped = true;
	_state.enabled = false;
	_state.speed = 0;
	digitalWrite(ENABLE, HIGH);
}

/*constexpr Drv8825 DRV8825(int ENABLE, int STEP, int SLEEP, int RESET) {
	//return (ENABLE >= 2 && ENABLE <= 13 || ENABLE >= 44 && ENABLE <= 46) ? Drv8825(ENABLE, STEP, SLEEP, RESET) : throw std::exception("Enable pin must be a PWM port");
	assert(ENABLE >= 2 && ENABLE <= 13 || ENABLE >= 44 && ENABLE <= 46, "Enable pin must be a PWM port");
	return Drv8825(ENABLE, STEP, SLEEP, RESET);
}*/
