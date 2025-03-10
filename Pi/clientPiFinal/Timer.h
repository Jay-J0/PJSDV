
#ifndef TIMER_H_
#define TIMER_H_

#include <sys/time.h>

class Timer {
public:
	Timer();
	void reset();
	void begin();
	void stop();
	unsigned int getTime() const;

private:
    long startTime,endTime;
};

#endif 
