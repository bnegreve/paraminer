/*
 *  TimeTracker.h
 *  GLCM
 *
 *  Created by Nicolas Sicard on 22/04/10.
 *  Copyright 2010 EFREI. All rights reserved.
 *
 */

#include <sys/time.h>
#include <unistd.h>  

#ifndef MEM_TIME_H
#define MEM_TIME_H

#define microsec 1000000.0

class TimeTracker {
private:
	struct timeval start_time;
	struct timeval stop_time;
	bool           running;
	
public:
	TimeTracker() {  	
		running=false;
	}
	
	void Start() {
		gettimeofday(&start_time, (struct timezone *) 0);
		running=true;
	}
	
	double Stop() {
		double st, en;
		if (!running) 
			return(-1.0);
		else {
			gettimeofday(&stop_time, (struct timezone *)0);
			st  = start_time.tv_sec + (start_time.tv_usec/microsec);
			en = stop_time.tv_sec + (stop_time.tv_usec/microsec);
			running=false;
			return (double)(en - st);
		}
	}
	
	bool Running() { return running; }
};

#endif /* MEM_TIME_H */