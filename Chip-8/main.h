#pragma once


inline extern void error(const char* msg);
inline extern void quit();

namespace game  {
	static bool Running = false;
	static bool keys[16];
	static float delay = 1;
}