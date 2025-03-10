
#include "Timer.h"

Timer::Timer():startTime(0),endTime(0) {

}


void Timer::reset() {
      startTime=0;
      endTime=0;
}

void Timer::begin() {
	struct timeval time;
	 gettimeofday(&time,0);
	 startTime=time.tv_sec;
}

void Timer::stop() {
	struct timeval time;
	 gettimeofday(&time,0);
	 endTime=time.tv_sec;
}

unsigned int Timer::getTime() const {
	return endTime-startTime;
}

