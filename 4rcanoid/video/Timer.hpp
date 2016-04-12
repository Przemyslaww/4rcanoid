#pragma once

class Timer {
	int startTicks;
	public:
		Timer() : startTicks(0) {}

		void start() {
			startTicks = SDL_GetTicks();
		}

		int getTicks() { return SDL_GetTicks() - startTicks; }

		void limitFramerate(int framesPerSecond, int screenTicksPerFrame) {
			if (getTicks() < screenTicksPerFrame)
			{
				SDL_Delay((screenTicksPerFrame) - getTicks());
			}
		}
};