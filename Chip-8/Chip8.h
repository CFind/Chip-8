#pragma once
#include <stdint.h>
class Chip8
{

	float timers_interval = 0;
	float last_timer_update = 0;
	uint16_t opcode = 0;
	/*Memory Map as follows
	0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
	0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
	0x200-0xFFF - Program ROM and work RAM
	*/
	uint8_t memory[4096] = {};
	//CPU registers. 0-15
	uint8_t v[16] = {};
	//Index register.
	uint16_t I = 0;
	//Program counter
	uint16_t pc = 0;
	//Counts down at 60hz when set above 0.
	uint8_t delay_timer = 0;
	//Buzzer sounds when this reaches 0.
	uint8_t sound_timer = 0;
	//The stack stores the program counter value before a jump is made 
	//or a subroutine is called.
	uint16_t stack[16] = {};
	//The stack pointer stores the current level of the stack.
	uint16_t stack_pointer = 0;


	bool drawFlag = false;


	void op_00E0_CLS();
	void op_00EE_RET();
	void op_1nnn_JP();
	void op_2nnn_CALL();
	void op_3xkk_SE();
	void op_4xkk_SNE();
	void op_5xy0_SE();
	void op_6xkk_LD();
	void op_7xkk_ADD();
	void op_8xy0_LD();
	void op_8xy1_OR();
	void op_8xy2_AND();
	void op_8xy3_XOR();
	void op_8xy4_ADD();
	void op_8xy5_SUB();
	void op_8xy6_SHR();
	void op_8xy7_SUBN();
	void op_8xyE_SHL();
	void op_9xy0_SNE();
	void op_Annn_LD();
	void op_Bnnn_JP();
	void op_Cxkk_RND();
	void op_Dxyn_DRW();
	void op_Ex9E_SKP();
	void op_ExA1_SKNP();
	void op_Fx07_LD();
	void op_Fx0A_LD();
	void op_Fx15_LD();
	void op_Fx18_LD();
	void op_Fx1E();
	void op_Fx29();
	void op_Fx33();
	void op_Fx55();
	void op_Fx65();
	void op_Table0();
	void op_Table8();
	void op_TableE();
	void op_TableF();
	void op_NULL();

	typedef void (Chip8::* FunctPointer)();
	FunctPointer op_Array[16] = { &Chip8::op_NULL };
	FunctPointer op0_Array[15] = { &Chip8::op_NULL };
	FunctPointer opE_Array[15] = { &Chip8::op_NULL };
	FunctPointer op8_Array[15] = { &Chip8::op_NULL };
	FunctPointer opF_Array[0x66] = { &Chip8::op_NULL };

public:
	Chip8();
	void loadROM(const char* filePath);
	void emulateCycle();
	void timer_update();
	//Pixel array. 2048 pixels
	bool graphics[64u * 32u] = {false};
	//Keypad states. 16 keys 0x0 - 0xF values.
	bool keys[16] = {};
	bool shouldDraw();
};