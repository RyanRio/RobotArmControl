#include "GPIO.h"
#include "pthread.h"

#ifndef SERVO_H
#define SERVO_H
class Servo {
	private:
		GPIO* gpio;
		int angle;
	public:
		Servo(GPIO* gpio,int angle);
		void single_cycle();
		void moveServo(int angle, int dps);
		int getAngle();
};
#endif

