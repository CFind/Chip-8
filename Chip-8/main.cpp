#include "Platform.h"
#include <chrono>
#include <string>
#include "main.h"
#include "Chip8.h"
#include <iostream>

using namespace std;


const char* filePath;


int main(int argc, char* argv[])
{
	using namespace chrono;
	float time_interval = 5;
	high_resolution_clock::time_point last_cycle = chrono::high_resolution_clock::now();

	Platform platform(1024,512);
	Chip8 chip8;

	//Pointer to the rom
	/*
	if (argc <= 1 || !isalpha(argv[1][0]))
		error("Need ROM filepath as string");
		*/

	chip8.loadROM(argv[1]);

	while (1) 
	{
		platform.getInput(chip8.keys);
		if (game::Running)
		{
			time_interval = duration<float, milliseconds::period>(high_resolution_clock::now() - last_cycle).count();
			if (time_interval > game::delay)
			{
				last_cycle = high_resolution_clock::now();
				chip8.emulateCycle();
			}
		}
		else {

		}

		platform.update(chip8.graphics, 64u, 32u, chip8.shouldDraw());
		
		

	}
	return 0;
}

void error(const char* msg)
{
	cerr<<"\nFATAL ERROR: "<< msg;
	exit(EXIT_FAILURE);
}

void quit() 
{
	exit(EXIT_SUCCESS);
}