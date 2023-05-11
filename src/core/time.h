#ifndef TIME_H
#define TIME_H

struct Time {
	static float deltaTime();
	static void beginFrame();
	
private:
	static float delta_time;
	static float previous_time;
};

#endif // TIME_H
