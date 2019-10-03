#ifndef TASK_HAND_H
#define TASK_HAND_H

class Task {
	public:
		Task(){}
		~Task(){}
		virtual void run() = 0;
};

#endif
