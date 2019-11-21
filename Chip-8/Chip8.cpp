#include <stdint.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include "Chip8.h"
#include "main.h"
using namespace std;


#pragma once
#define VF 0xF
#define VIDEO_WIDTH 64u
#define VIDEO_HEIGHT 32u
#define FONTSET_SIZE 80u
#define FONTSET_START_ADDRESS 0x50u
#define PROGRAM_LOC 512u




uint8_t fontset[FONTSET_SIZE] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

bool quit_emulator = false;
//Chip-8 has 35 opcodes 2 bytes in length.
//This holds the current opcode.






Chip8::Chip8()
{

	for (int i = 0; i < FONTSET_SIZE; i++)
			memory[FONTSET_START_ADDRESS + i] = fontset[i];

	op_Array[0x0] = &Chip8::op_Table0;
	op_Array[0x1] = &Chip8::op_1nnn_JP;
	op_Array[0x2] = &Chip8::op_2nnn_CALL;
	op_Array[0x3] = &Chip8::op_3xkk_SE;
	op_Array[0x4] = &Chip8::op_4xkk_SNE;
	op_Array[0x5] = &Chip8::op_5xy0_SE;
	op_Array[0x6] = &Chip8::op_6xkk_LD;
	op_Array[0x7] = &Chip8::op_7xkk_ADD;
	op_Array[0x8] = &Chip8::op_Table8;
	op_Array[0x9] = &Chip8::op_9xy0_SNE;
	op_Array[0xA] = &Chip8::op_Annn_LD;
	op_Array[0xB] = &Chip8::op_Bnnn_JP;
	op_Array[0xC] = &Chip8::op_Cxkk_RND;
	op_Array[0xD] = &Chip8::op_Dxyn_DRW;
	op_Array[0xE] = &Chip8::op_TableE;
	op_Array[0xF] = &Chip8::op_TableF;
	op0_Array[0] = &Chip8::op_00E0_CLS;
	op0_Array[0xE] = &Chip8::op_00EE_RET;
	opE_Array[0x1] = &Chip8::op_ExA1_SKNP;
	opE_Array[0xE] = &Chip8::op_Ex9E_SKP;
	op8_Array[0] = &Chip8::op_8xy0_LD;
	op8_Array[0x1] = &Chip8::op_8xy1_OR;
	op8_Array[0x2] = &Chip8::op_8xy2_AND;
	op8_Array[0x3] = &Chip8::op_8xy3_XOR;
	op8_Array[0x4] = &Chip8::op_8xy4_ADD;
	op8_Array[0x5] = &Chip8::op_8xy5_SUB;
	op8_Array[0x6] = &Chip8::op_8xy6_SHR;
	op8_Array[0x7] = &Chip8::op_8xy7_SUBN;
	op8_Array[0xE] = &Chip8::op_8xyE_SHL;
	opF_Array[0x07] = &Chip8::op_Fx07_LD;
	opF_Array[0x0A] = &Chip8::op_Fx0A_LD;
	opF_Array[0x15] = &Chip8::op_Fx15_LD;
	opF_Array[0x18] = &Chip8::op_Fx18_LD;
	opF_Array[0x1E] = &Chip8::op_Fx1E;
	opF_Array[0x29] = &Chip8::op_Fx29;
	opF_Array[0x33] = &Chip8::op_Fx33;
	opF_Array[0x55] = &Chip8::op_Fx55;
	opF_Array[0x65] = &Chip8::op_Fx65;
	pc = 0x200;
	opcode = 0;
	I = 0;
	stack_pointer = 0;
}

void Chip8::emulateCycle()
{
	//fetch opcode
	opcode = memory[pc] << 8u | memory[pc + 1];

	pc += 2;
	cout << "\nCurrent Op: " << hex <<  opcode << dec;
	this->timer_update();
	(this->*op_Array[(opcode >> 12) & 0x000Fu])();
}


bool Chip8::shouldDraw() 
{
	if (drawFlag) {
		drawFlag = false;
		return true;
	}
	return false;
}


void Chip8::timer_update() 
{
	timers_interval = ((clock() - last_timer_update) / CLOCKS_PER_SEC) * 1000;
	if (timers_interval >= 16.666667) {
		last_timer_update = clock();
		if (delay_timer > 0)
			delay_timer--;
		if (sound_timer > 0)
			sound_timer--;
	}
}

void Chip8::loadROM(const char* filePath) 
{
	fstream fs;
	try
	{
		fs.open(filePath, fstream::binary | fstream::in | fstream::ate);
		double length = fs.tellg();
		fs.seekg(0, fs.beg);
		char* buffer = new char[length];
		fs.read(buffer, length);

		fs.close();
		for (int i = 0; i < length && i <= 4095; ++i) {
			memory[0x200 + i] =  buffer[i];
		}
		delete[] buffer;

	}
	catch (const fstream::failure& e)
	{
		error(e.what());
	}
	catch (const std::exception & e) 
	{
		error(e.what());
	}

	
	
	/*FILE* pFile = nullptr;
	fopen_s(&pFile, filePath, "rb");
	char* buffer;
	long size;
	long result;

	if (pFile == NULL)
		error("ROM not found!");


	fseek(pFile, 0, SEEK_END);
	size = ftell(pFile);
	rewind(pFile);

	buffer = (char*)malloc(sizeof(char) * size);
	if (buffer == NULL)
		error("Load ROM memory allocation error.");

	result = fread(buffer, 1, size, pFile);
	if (result != size)
		error("File read error!");

	for (int i = 0; i < size; i++)
		memory[i + PROGRAM_LOC] = buffer[i];

	fclose(pFile);
	free(buffer);*/

}

////Opcode functions
//Clear the display.
void Chip8::op_00E0_CLS() {
	memset(Chip8::graphics, 0, sizeof(Chip8::graphics));
}
//Return from a subroutine.
void Chip8::op_00EE_RET() {
	stack_pointer--;
	pc = stack[stack_pointer];
}
//The interpreter sets the program counter to nnn
void Chip8::op_1nnn_JP() {
	uint16_t address = opcode & 0x0FFFu;
	pc = address;
}
//The interpreter increments the stack pointer, 
//then puts the current PC on the top of the stack. The PC is then set to nnn.
void Chip8::op_2nnn_CALL() {
	uint16_t address = opcode & 0x0FFFu;
	stack[stack_pointer] = pc;
	stack_pointer++;
	pc = address;
}
//The interpreter compares register Vx to kk, 
//and if they are equal, increments the program counter by 2.
void Chip8::op_3xkk_SE() {
	uint8_t Vx = (opcode >> 8) & 0x000Fu;
	uint8_t byte = opcode & 0x00FF;
	if (v[Vx] == byte)
		pc += 2;
}
//The interpreter compares register Vx to kk, 
//and if they are not equal, increments the program counter by 2.
void Chip8::op_4xkk_SNE() {
	uint8_t Vx = (opcode >> 8) & 0x000Fu;
	uint8_t byte = opcode & 0x00FF;
	if (v[Vx] != byte)
		pc += 2;
}
//The interpreter compares register Vx to register Vy, 
//and if they are equal, increments the program counter by 2.
void Chip8::op_5xy0_SE() {
	uint8_t Vx = (opcode >> 8) & 0x000Fu;
	uint8_t Vy = (opcode >> 4) & 0x000Fu;
	if (v[Vx] == v[Vy])
		pc += 2;
}
//The interpreter puts the value kk into register Vx.
void Chip8::op_6xkk_LD() {
	uint8_t Vx = (opcode >> 8) & 0x000Fu;
	uint8_t byte = opcode & 0x00FF;

	v[Vx] = byte;
}
//Adds the value kk to the value of register Vx, then stores the result in Vx. 
void Chip8::op_7xkk_ADD() {
	uint8_t Vx = (opcode >> 8) & 0x000Fu;
	uint8_t byte = opcode & 0x00FF;
	v[Vx] += byte;
}
//Stores the value of register Vy in register Vx.
void Chip8::op_8xy0_LD() {
	uint8_t Vx = (opcode >> 8) & 0x000Fu;
	uint8_t Vy = (opcode >> 4) & 0x000Fu;

	v[Vx] = v[Vy];
}
//Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx.
void Chip8::op_8xy1_OR() {
	uint8_t Vx = (opcode >> 8) & 0x000Fu;
	uint8_t Vy = (opcode >> 4) & 0x000Fu;

	v[Vx] = v[Vx] | v[Vy];
}
//Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx.
void Chip8::op_8xy2_AND() {
	uint8_t Vx = (opcode >> 8) & 0x000Fu;
	uint8_t Vy = (opcode >> 4) & 0x000Fu;

	v[Vx] = v[Vx] & v[Vy];
}
//Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx.
void Chip8::op_8xy3_XOR() {
	uint8_t Vx = (opcode >> 8) & 0x000Fu;
	uint8_t Vy = (opcode >> 4) & 0x000Fu;

	v[Vx] = v[Vx] ^ v[Vy];
}
//The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,) 
//VF is set to 1, otherwise 0.
void Chip8::op_8xy4_ADD() {
	uint8_t Vx = (opcode >> 8) & 0x000Fu;
	uint8_t Vy = (opcode >> 4) & 0x000Fu;

	uint16_t val = v[Vx] + v[Vy];
	v[15] = (val > 255u);

	v[Vx] = val & 0xFFu;

}
//If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
void Chip8::op_8xy5_SUB() {
	uint8_t Vx = (opcode >> 8) & 0x000Fu;
	uint8_t Vy = (opcode >> 4) & 0x000Fu;

	v[0xF] = (v[Vx] > v[Vy]);
	v[Vx] -= v[Vy];
}
//If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
void Chip8::op_8xy6_SHR() {
	uint8_t Vx = (opcode >> 8) & 0x000Fu;
	uint8_t Vy = (opcode >> 4) & 0x000Fu;

	v[0xF] = (v[Vx] & 1u);

	v[Vx] /= 2;
}
//If Vy > Vx, then VF is set to 1, otherwise 0.
//Then Vx is subtracted from Vy, and the results stored in Vx.
void Chip8::op_8xy7_SUBN() {
	uint8_t Vx = (opcode >> 8) & 0x000Fu;
	uint8_t Vy = (opcode >> 4) & 0x000Fu;

	v[VF] = (v[Vy] > v[Vx]);

	v[Vx] = v[Vy] - v[Vx];
}
//If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
void Chip8::op_8xyE_SHL() {
	uint8_t Vx = (opcode >> 8) & 0x000Fu;

	v[VF] = ((v[Vx] >> 7) & 1u);

	v[Vx] *= 2;

}
//The values of Vx and Vy are compared, and if they are not equal, the program counter is increased by 2.
void Chip8::op_9xy0_SNE() {
	uint8_t Vx = (opcode >> 8) & 0x000Fu;
	uint8_t Vy = (opcode >> 4) & 0x000Fu;

	if (v[Vx] != v[Vy])
		pc += 2;
}
//The value of register I is set to nnn.
void Chip8::op_Annn_LD() {
	uint16_t address = opcode & 0x0FFFu;

	I = address;
}
//The program counter is set to nnn plus the value of V0.
void Chip8::op_Bnnn_JP() {
	uint16_t address = opcode & 0x0FFFu;

	pc = address + v[0];
}
//The interpreter generates a random number from 0 to 255, 
//which is then ANDed with the value kk. The results are stored in Vx.
void Chip8::op_Cxkk_RND() {
	uint8_t Vx = (opcode >> 8) & 0x000Fu;
	uint8_t byte = (opcode & 0x00FF);

	srand(clock());
	uint8_t rNum = rand() * 255;

	v[Vx] = rNum & byte;
}
//The interpreter reads n bytes from memory, starting at the address stored in I. 
//These bytes are then displayed as sprites on screen at coordinates (Vx, Vy).
void Chip8::op_Dxyn_DRW() {
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	uint8_t height = opcode & 0x000Fu;

	// Wrap if going beyond screen boundaries
	uint8_t xPos = v[Vx] % VIDEO_WIDTH;
	uint8_t yPos = v[Vy] % VIDEO_HEIGHT;

	v[0xF] = 0;


	for (unsigned int row = 0; row < height; ++row)
	{
		uint8_t spriteByte = memory[I + row];

		for (unsigned int col = 0; col < 8; ++col)
		{
			uint8_t spritePixel = spriteByte & (0x80u >> col);
			bool* screenPixel = &graphics[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

			// Sprite pixel is on
			if (spritePixel)
			{
				// Screen pixel also on - collision
				if (*screenPixel)
				{
					v[0xF] = 1;
				}

				// Effectively XOR with the sprite pixel
				*screenPixel ^= 1;
			}
		}
	}
	drawFlag = true;
}
//Skip next instruction if key with the value of Vx is pressed.
void Chip8::op_Ex9E_SKP() {
	uint8_t Vx = (opcode >> 8) & 0x000Fu;
	uint8_t key = v[Vx];
	if (keys[key])
		pc += 2;
}
//Skip next instruction if key with the value of Vx is not pressed.
void Chip8::op_ExA1_SKNP() {
	uint8_t Vx = (opcode >> 8) & 0x000Fu;
	uint8_t key = v[Vx];
	if (!keys[key])
		pc += 2;
}
//Set Vx = delay timer value.
//The value of DT is placed into Vx.
void Chip8::op_Fx07_LD() {
	uint8_t Vx = (opcode >> 8) & 0x000Fu;
	v[Vx] = delay_timer;
}
//All execution stops until a key is pressed, then the value of that key is stored in Vx.
void Chip8::op_Fx0A_LD() {
	uint8_t Vx = (opcode >> 8) & 0x000Fu;
	for (int i = 0; i <= 15; i++)
		if (keys[i]) {
			v[Vx] = i;
			return;
		}
	pc -= 2;
}
//Set delay timer = Vx.
void Chip8::op_Fx15_LD() {
	uint8_t Vx = (opcode >> 8) & 0x000Fu;
	delay_timer = v[Vx];
}
//Set sound timer = Vx.
void Chip8::op_Fx18_LD() {
	uint8_t Vx = (opcode >> 8) & 0x000Fu;
	sound_timer = v[Vx];
}
//Set I = I + Vx.
void Chip8::op_Fx1E() {
	uint8_t Vx = (opcode >> 8) & 0x000Fu;
	I += v[Vx];
}
//Set I = location of sprite for digit Vx.
void Chip8::op_Fx29() {
	uint8_t Vx = (opcode >> 8) & 0x000Fu;
	I = FONTSET_START_ADDRESS + (5 * v[Vx]);
}
//Store BCD representation of Vx in memory locations I, I+1, and I+2.
void Chip8::op_Fx33() {
	uint8_t Vx = (opcode >> 8) & 0x000Fu;
	uint8_t val = v[Vx];

	memory[I + 2] = val % 10;
	val /= 10;
	memory[I + 1] = val % 10;
	val /= 10;
	memory[I] = val % 10;
}
//Store registers V0 through Vx in memory starting at location I.
void Chip8::op_Fx55() {
	uint8_t Vx = (opcode >> 8) & 0x000Fu;
	for (int i = 0; i <= Vx; i++) {
		memory[I + i] = v[i];
	}
}
//Read registers V0 through Vx from memory starting at location I.
void Chip8::op_Fx65() {
	uint8_t Vx = (opcode >> 8) & 0x000Fu;
	for (uint8_t i = 0; i <= Vx; i++)
	{
		v[i] = memory[I + i];
	}
}

void Chip8::op_Table0() {
	(this->*op0_Array[opcode & 0x000Fu])();
}
void Chip8::op_Table8() {
	uint8_t opLSD = opcode & 0xFu;
	(this->*op8_Array[opLSD])();
}
void Chip8::op_TableE() {
	(this->*opE_Array[opcode & 0x000Fu])();
}
void Chip8::op_TableF() {
	(this->*opF_Array[opcode & 0x00FFu])();
}
void Chip8::op_NULL() {
	error("Null Opcode!");
}