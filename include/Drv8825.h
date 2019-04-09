#define DRV8825(ENABLE, STEP, SLEEP, RESET) ([]{ \
	static_assert((ENABLE >= 2 && ENABLE <= 13) || (ENABLE >= 44 && ENABLE <= 46), "Enable pin must be a PWM port"); \
static_assert(ENABLE >= 2 && STEP >= 2 && SLEEP >= 2 && RESET >= 2, "Pins 0 and 1 are reserved for serial communication"); \
	}(), \
	Drv8825(ENABLE, STEP, SLEEP, RESET))

enum Dir {
	CCW = -1,
	Stop = 0,
	CW = 1
};
/* STEP MAP: (+ is CW) (A is near supply, +s are on outside)
step:   A:   B:
   0:  100    0
   1:   71   71  START POS
   2:    0  100
   3:  -71   71
   4: -100    0
   5:  -71  -71
   6:    0 -100
   7:   71  -71
*/

class Drv8825 {
	public:
		struct State {
			bool stopped = true; //controlled by user
			bool enabled = false; //actual pin (user + dir)
			int speed = 255;
			int step = 1;
			bool slept = false;
			bool reset = false;
			Dir motor1 = Dir::CW;
			Dir motor2 = Dir::CW;
		};
		Drv8825(int enable_pin, int step_pin, int sleep_pin, int reset_pin);
		State getState() const;
		void begin();
		void sleep();
		void wake();
		void reset();
		void run(int);
		void stop();
		void dir(int motor, Dir dir);
		void dir(Dir motor1, Dir motor2);
	private:
		void step(int num);
		const int ENABLE;
		const int STEP;
		const int SLEEP;
		const int RESET;
		State _state;
};
