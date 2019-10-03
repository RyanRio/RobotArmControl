
CFLAGS = -g  -std=c++0x -O3 

main: ZedBoard.o ZedBoard.h Servo.o GPIO.o GPIO.h task.h barrier.o
	g++ $(CFLAGS) Servo.o GPIO.o barrier.o ZedBoard.o -o main -lpthread

Servo.o: Servo.cpp
	g++ $(CFLAGS) -c Servo.cpp -o Servo.o

barrier.o: task.h barrier.cpp barrier.h
	g++ $(CFLAGS) -c barrier.cpp -o barrier.o

GPIO.o: GPIO.h GPIO.cpp
	g++ $(CFLAGS) -c GPIO.cpp -o GPIO.o

ZedBoard.o: ZedBoard.cpp ZedBoard.h
	g++ $(CFLAGS) -c ZedBoard.cpp -o ZedBoard.o

clean: 
	rm *.o
	rm main
PHONY: clean
