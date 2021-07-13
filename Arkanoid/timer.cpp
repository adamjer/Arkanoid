#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<stdio.h>
#include"headers.h"

/*
class Timer{
public:
Timer();

void start();

Uint32 getTicks();

bool isStarted();

private:
Uint32 startTicks;

bool started;
};
*/

Timer::Timer(){
	startTicks = 0;

	started = false;
}


void Timer::start(){
	started = true;


	startTicks = SDL_GetTicks();
}


void Timer::stop(){
	started = false;

	startTicks = 0;
}


Uint32 Timer::getTicks(){
	Uint32 time = 0;

	if (started){
		time = (SDL_GetTicks() - startTicks)/1000;	
	}
	return time;
}


bool Timer::isStarted(){
	return started;
}
