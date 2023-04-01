#include "Singleton.h"
#include <string>
#include <chrono>

using namespace std;
using namespace chrono;

#ifndef timeController_h
#define timeController_h

namespace PM3D {
	class TimeController : public CSingleton <TimeController> {
		time_point<chrono::system_clock> start;
		time_point<chrono::system_clock> pauseStart;

	public:
		void startTimer() {
			start = chrono::system_clock::now();
		}

		auto getTimeElapsed() {
			return system_clock::now() - start; 
		}

		void pause() {
			pauseStart = system_clock::now();
		}

		void resume() {
			start = start + (system_clock::now() - pauseStart);
		}

		void restart() {
			start = system_clock::now();
		}

	

	};
}
#endif