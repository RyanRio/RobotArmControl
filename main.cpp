#include "GPIO.h"
#include <iostream>
#include <cmath>
using namespace std;
int main()
{

int servoNum;
cout << "Servo 1: Base" << endl;
cout << "Servo 2: Bicep" << endl;
cout << "Servo 3: Elbow" << endl;
cout << "Servo 4: Wrist" << endl;
cout << "Servo 5: Gripper" << endl;
cout << "Enter a servo number between 1 and 5: ";
cin >> servoNum;

switch (servoNum) {

	case 1:
		servoNum = 13;
		break;
        case 2:
                servoNum = 10;
                break;
        case 3:
                servoNum = 11;
                break;
        case 4:
                servoNum = 12;
                break;
        case 5:
                servoNum = 0;
                break;
}
int degrees;
cout << "Enter a start position for the servo, as an angle in degrees:";
cin >> degrees;

if(degrees < 20) {
	degrees=20;
}

if(degrees > 160) {
	degrees=160;
}

int microStart = degrees*10 + 600;

int degreesEnd;
cout << "Enter a end position for the servo, as an angle in degrees:";
cin >> degreesEnd;

if(degreesEnd < 20) {
        degreesEnd=20;
}

if(degreesEnd > 160) {
        degreesEnd=160;
}

int microEnd = degreesEnd*10 + 600;

int speed;
cout << "Enter a rotational speed, given in degrees per second: ";
cin >> speed;

if(speed <= 0) {
 speed = 1;
}

int num_periods = (int)(abs((double)(degreesEnd-degrees)));
num_periods = num_periods/speed;
num_periods = num_periods * 100 / 2;
 
// Open device file 13 on Linux file system
GPIO gpio(servoNum);
// Generate PWM signal with 20ms period and 1.5ms on time.
// Generate 400 periods, this will take 20ms * 400 iterations = 8s
gpio.GeneratePWM(20000, microStart, 50);
gpio.GenerateVariablePWM(20000, microStart, microEnd, num_periods);
// Done
return 0;
}
