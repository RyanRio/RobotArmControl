#include "Servo.h"
#include "GPIO.h"
#include <iostream>
#include "barrier.h"
#include <pthread.h>
#include <cmath>
#include <cstring>
#include "ZedBoard.h"

using namespace std;

typedef struct MoveArgs {
	Servo* gpio;
	int angle;
	int speed;
	barrier* stop;
} MoveArgs;

typedef struct movement {
	int endAngle[5];
	int dps[5];
} movement;

Servo::Servo(GPIO* gpio, int angle) {
	this->gpio = gpio;
	this->angle = angle;
}

int Servo::getAngle() {
	return this->angle;
}

void Servo::single_cycle() {
	int micro = this->angle*10 + 600;
	this->gpio->GeneratePWM(20000, micro, 1);
}
void Servo::moveServo(int degreesEnd, int dps) {
	int microStart = this->angle*10 + 600;
	int microEnd = degreesEnd*10 + 600;

	if(dps <= 0) {
		dps = 1;
	}

	int num_periods = (int)(abs((double)(degreesEnd-this->angle)));
	num_periods = num_periods*50/dps;
	//num_periods = num_periods * 100 / 2;
	this->gpio->GenerateVariablePWM(20000, microStart, microEnd, num_periods);
	this->angle = degreesEnd;
}

class Routine {
	private:
		Servo* servos;
	public:
		Routine(Servo* servos);
		void preformMovements(movement* m, int n);
		void doMovement(movement m);
};

Routine::Routine(Servo* servos) {
	this->servos = servos;
}


void Routine::preformMovements(movement* m, int n) {
	for (int i = 0; i < n; i++) {
		doMovement(m[i]);
	}
}

class HoldPosition: public Task {
	private:
		Servo* runner;
	public:
		HoldPosition(Servo* serv){
			runner = serv;
		}

		~HoldPosition(){}

		virtual void run() {
			runner->single_cycle();
		}

};

void* run_movement(void* arg) {
	MoveArgs* val = (MoveArgs *) arg;
	
	if (val->angle != val->gpio->getAngle())  {
		val->gpio->moveServo(val->angle,val->speed);
	}
	HoldPosition* h = new HoldPosition(val->gpio); 
	barrier_wait(val->stop,h);
	return NULL;
}

// l represents the number of movements
void Routine::doMovement(movement m) {
	pthread_t ids[5];
	barrier* b = make_barrier(5);
	MoveArgs arguments[5];
	for(int k=0; k<5; k++) {
		arguments[k] = MoveArgs{&this->servos[k],m.endAngle[k],m.dps[k],b};
		pthread_create(&ids[k],NULL,run_movement, &arguments[k]);
	}

	for (int i = 0; i < 5; i++) {
		void* ret;
		pthread_join(ids[i],&ret );
	}


	free_barrier(b);
}


movement* read_file(FILE* in, int *out) {

	movement* total = (movement*)malloc(sizeof(movement));
	int cap = 1;
	int nums = 0;		

	char buff[200]; 
	while (NULL != fgets(buff,200,in)) {
		int angles[5];
		int speeds[5];
		if (buff[0] != '/') {
        angles[0] = atoi(strtok(buff," "));
		for (int i = 1; i < 5;i++) {
			angles[i] = atoi(strtok(NULL," "));
		} 

		for (int i =0; i<5;i++) {
			speeds[i] = atoi(strtok(NULL," "));
		}


		if (nums>= cap) {
			cap = cap*2;
			total = (movement*)realloc(total,sizeof(movement)*cap);
		}

		memcpy(total[nums].endAngle,angles,sizeof(int)*5);
		memcpy(total[nums].dps,speeds,sizeof(int)*5);
		nums++;
        }
	}

	*out = nums;
	return total; 

}

void free_movement(movement* m) {
	free(m);
}


movement read_init_state(FILE* in, int* out) {
		char buff[200]; 
		fgets(buff,200,in);
		movement m;
		int angles[5];
		out[0] = atoi(strtok(buff," "));
		for (int i = 1; i < 5;i++) {
			out[i] = atoi(strtok(NULL," "));
		}
		
		fgets(buff,200,in);
		m.endAngle[0] = atoi(strtok(buff," "));
		for (int i = 1; i < 5;i++) {
			m.endAngle[i] = atoi(strtok(NULL," "));
		}

		for (int i = 0; i < 5;i++) {
			m.dps[i] = atoi(strtok(NULL," "));
		}
		
		return m;
}


int main(int argc, char* argv[])
{
	if(argc<3) {
		cout << "Error: did not give file name" << endl;
		return 1;
	}

	FILE* in = fopen(argv[1], "r");
	FILE* init_state = fopen(argv[2],"r");
	int out =0;
	movement* movs = read_file(in,&out);
	GPIO* wrist = new GPIO{12};
	GPIO* gripper = new GPIO{0};
	GPIO* base = new GPIO{13};
	GPIO* bicep = new GPIO{10};
	GPIO* elbow = new GPIO{11};


	int initAng[5];
	movement m = read_init_state(init_state,initAng);
	Servo wristS(wrist, initAng[0]);
	Servo gripperS(gripper, initAng[1]);
	Servo baseS(base,initAng[2]);
	Servo bicepS(bicep,initAng[3]);
	Servo elbowS(elbow,initAng[4]);

	movement first[1];
	first[0] = m;
	Servo robot[5] = {wristS, gripperS, baseS, bicepS, elbowS};
	Servo* allServos = robot;

	
	Routine r =  Routine(allServos);
	r.preformMovements(first,1);
	bool init = true;
	ZedBoard zb = ZedBoard();
	while(init) {
	//	for (int i =  0; i <5;i++) {
	//		robot[i].single_cycle();	
	//	}
		robot[0].single_cycle();
		robot[2].single_cycle();
		robot[3].single_cycle();
		robot[4].single_cycle();	
		//TODO CHECK FOR BUTTON HERE UPDATE init
		if (zb.PushButtonGet() != 0) {
			init = false;
		}
		
	}

	r.preformMovements(movs,out);
	free_movement(movs);	    
	
    for(;;) {
        robot[0].single_cycle();
		robot[3].single_cycle();
		robot[4].single_cycle();	
        robot[2].single_cycle();
	 
    }
    return 0;
}
