//
// Created by Dimitri on 05/09/2025.
//

#ifndef CHIP8_CPP_CLION_CHIP8_H
#define CHIP8_CPP_CLION_CHIP8_H


#pragma once
#include <fstream>
#include <iostream>
#include <random>
#include <cstring>
#include <filesystem>
#include <vector>

class Chip8
{
	public:
		using Instruction = void (Chip8::*)();
		Instruction instructions[0xF + 1];
		Instruction table0[0xE + 1];
		Instruction table8[0xE + 1];
		Instruction tableE[0xE + 1];
		Instruction tableF[0x65 + 1];

		static const unsigned int PROGRAM_START_ADDRESS = 0x200;
		static const unsigned char FONT_START_ADDRESS = 0x50;
		static const unsigned char FONT_SIZE = 80;
		static const int DISPLAY_WIDTH = 64;
		static const int DISPLAY_HEIGHT = 32;
		int	program_counter{};
		int I{}; //Index register
		int stack[16]{};
		unsigned char stack_pointer{};
		unsigned char delay_timer{};
		unsigned char sound_timer{};
		unsigned char registers[16]{}; //16 registers from 0 to F (V0 -> VF)
		unsigned char memory[4096]{}; //Font is loaded from 000 to 1FF (convention is 050 to 1FF), programs are loaded from 200 (512 in decimal)
		unsigned char display[64 * 32]{};
		unsigned char keypad[16]{};
		unsigned short opcode {};
		unsigned char font[FONT_SIZE] = {
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

		Chip8()
		{
			initializeInstructions();

			initializeTable0();

			initializeTable8();

			initializeTableE();

			initializeTableF();

			program_counter = 0x200;

			//Load font
			for (int i = 0; i < FONT_SIZE; i++)
			{
				memory[FONT_START_ADDRESS + i] = font[i];
			}

			memset(display, 0, sizeof display);
		}

		void LoadROM(std::string file_path)
		{
			std::ifstream ROM(file_path, std::ios::binary);
			std::cout << "Chemin absolu : " << std::filesystem::absolute(file_path) << std::endl;


			if (ROM.is_open())
			{
				ROM.seekg(0, std::ios::end);
				int size = ROM.tellg();
				ROM.seekg(0, std::ios::beg);

				char* buffer = new char[size];
				ROM.read(buffer, size);
				ROM.close();

				for (long i = 0;i < size;i++)
				{
					memory[PROGRAM_START_ADDRESS + i] = buffer[i];
				}

				delete[] buffer;
			}
			else
			{
				std::cout << "Le fichier n'est pas ouvert";
			}
		}

		void main_loop()
		{
			opcode = (memory[program_counter] << 8) | memory[program_counter + 1];

			program_counter += 2;

			(this->*instructions[(opcode & 0xF000) >> 12])();

			if (sound_timer > 0)
			{
				sound_timer--;
			}

			if (delay_timer > 0)
			{
				delay_timer--;
			}
		}
	private:
		static int generateRandomNumber(int min, int max) {
			static std::random_device rd;
			static std::mt19937 gen(rd());

			std::uniform_int_distribution<> distrib(min, max);
			return distrib(gen);
		}

		void initializeInstructions()
		{
			instructions[0x0] = &Chip8::Table0;
			instructions[0x1] = &Chip8::OP_1NNN;
			instructions[0x2] = &Chip8::OP_2NNN;
			instructions[0x3] = &Chip8::OP_3XNN;
			instructions[0x4] = &Chip8::OP_4XNN;
			instructions[0x5] = &Chip8::OP_5XY0;
			instructions[0x6] = &Chip8::OP_6XNN;
			instructions[0x7] = &Chip8::OP_7XNN;
			instructions[0x8] = &Chip8::Table8;
			instructions[0x9] = &Chip8::OP_9XY0;
			instructions[0xA] = &Chip8::OP_ANNN;
			instructions[0xB] = &Chip8::OP_BNNN;
			instructions[0xC] = &Chip8::OP_CXNN;
			instructions[0xD] = &Chip8::OP_DXYN;
			instructions[0xE] = &Chip8::TableE;
			instructions[0xF] = &Chip8::TableF;
		}

		void initializeTable0()
		{
			for (int i = 0;i <= 0xE;i++)
			{
				table0[i] = &Chip8::OP_NULL;
			}

			table0[0x0] = &Chip8::OP_00EO;
			table0[0xE] = &Chip8::OP_00EE;
		}

		void initializeTable8()
		{
			for (int i = 0;i <= 0xE;i++)
			{
				table8[i] = &Chip8::OP_NULL;
			}

			table8[0x0] = &Chip8::OP_8XY0;
			table8[0x1] = &Chip8::OP_8XY1;
			table8[0x2] = &Chip8::OP_8XY2;
			table8[0x3] = &Chip8::OP_8XY3;
			table8[0x4] = &Chip8::OP_8XY4;
			table8[0x5] = &Chip8::OP_8XY5;
			table8[0x6] = &Chip8::OP_8XY6;
			table8[0x7] = &Chip8::OP_8XY7;
			table8[0xE] = &Chip8::OP_8XYE;
		}

		void initializeTableE() {
			for (int i = 0;i < 0xE;i++) {
				tableE[i] = &Chip8::OP_NULL;
			}

			tableE[0xE] = &Chip8::OP_EX9E;
			tableE[0x1] = &Chip8::OP_EXA1;
		}

		void initializeTableF() {
			for (int i = 0;i < 0x65;i++) {
				tableF[i] = &Chip8::OP_NULL;
			}

			tableF[0x07] = &Chip8::OP_FX07;
			tableF[0x0A] = &Chip8::OP_Fx0A;
			tableF[0x15] = &Chip8::OP_FX15;
			tableF[0x18] = &Chip8::OP_FX18;
			tableF[0x1E] = &Chip8::OP_FX1E;
			tableF[0x29] = &Chip8::OP_FX29;
			tableF[0x33] = &Chip8::OP_FX33;
			tableF[0x55] = &Chip8::OP_FX55;
			tableF[0x65] = &Chip8::OP_FX65;
		}

		void Table0()
		{
			//Getting the correct function using function pointers
			(this->*table0[opcode & 0x000F])();
		}

		void Table8()
		{
			(this->*table8[opcode & 0x000F])();
		}

		void TableE() {
			(this->*tableE[opcode & 0x000F])();
		}

		void TableF() {
			(this->*tableF[opcode & 0x00FF])();
		}

		void OP_NULL()
		{}

		void OP_00EO()
		{
			memset(display, 0, sizeof display);
		}

		void OP_00EE()
		{
			--stack_pointer;
			program_counter = stack[stack_pointer];
		}

		void OP_1NNN()
		{
			unsigned short NNN = opcode & 0x0FFF;

			program_counter = NNN;
		}

		void OP_2NNN()
		{
			unsigned short NNN = opcode & 0x0FFF;

			stack[stack_pointer] = program_counter;
			stack_pointer++;

			program_counter = NNN;
		}

		void OP_3XNN()
		{
			unsigned short X = (opcode & 0x0F00) >> 8;
			unsigned short NN = opcode & 0x00FF;

			if (registers[X] == NN)
			{
				program_counter += 2;
			}
		}

		void OP_4XNN()
		{
			unsigned short X = (opcode & 0x0F00) >> 8;
			unsigned short NN = opcode & 0x00FF;

			if (registers[X] != NN)
			{
				program_counter += 2;
			}
		}

		void OP_5XY0()
		{
			unsigned short X = (opcode & 0x0F00) >> 8;
			unsigned short Y = (opcode & 0x00F0) >> 4;

			if (registers[X] == registers[Y])
			{
				program_counter += 2;
			}
		}

		void OP_6XNN()
		{
			unsigned short X = (opcode & 0x0F00) >> 8;
			unsigned char NN = opcode & 0x00FF;

			registers[X] = NN;
		}

		void OP_7XNN()
		{
			unsigned short X = (opcode & 0x0F00) >> 8;
			unsigned char NN = opcode & 0x00FF;

			registers[X] += NN;
		}

		void OP_8XY0()
		{
			unsigned char X = (opcode & 0x0F00) >> 8;
			unsigned char Y = (opcode & 0x00F0) >> 4;

			registers[X] = registers[Y];
		}

		void OP_8XY1()
		{
			unsigned char X = (opcode & 0x0F00) >> 8;
			unsigned char Y = (opcode & 0x00F0) >> 4;

			registers[X] = registers[X] | registers[Y];
		}

		void OP_8XY2()
		{
			unsigned char X = (opcode & 0x0F00) >> 8;
			unsigned char Y = (opcode & 0x00F0) >> 4;

			registers[X] = registers[X] & registers[Y];
		}

		void OP_8XY3()
		{
			unsigned char X = (opcode & 0x0F00) >> 8;
			unsigned char Y = (opcode & 0x00F0) >> 4;

			registers[X] ^= registers[Y];
		}

		void OP_8XY4()
		{
			unsigned char X = (opcode & 0x0F00) >> 8;
			unsigned char Y = (opcode & 0x00F0) >> 4;
			int sum = registers[X] + registers[Y];

			registers[X] += registers[Y];

			if (sum > 0xFF)
			{
				registers[0xF] = 1;
			}
			else
			{
				registers[0xF] = 0;
			}
		}

		void OP_8XY5()
		{
			unsigned short X = (opcode & 0x0F00) >> 8;
			unsigned short Y = (opcode & 0x00F0) >> 4;
			bool greater = registers[Y] > registers[X];
			registers[X] -= registers[Y];

			if (greater)
			{
				registers[0xF] = 0;
			}
			else
			{
				registers[0xF] = 1;
			}
		}

		void OP_8XY6()
		{
			unsigned short X = (opcode & 0x0F00) >> 8;
			unsigned short Y = (opcode & 0x00F0) >> 4;
			unsigned char Vx = registers[X];

			registers[X] >>= 1;

			registers[0xF] = Vx & 0x1;
		}

		void OP_8XY7()
		{
			unsigned short X = (opcode & 0x0F00) >> 8;
			unsigned short Y = (opcode & 0x00F0) >> 4;
			registers[X] = registers[Y] - registers[X];

			if (registers[Y] > registers[X])
			{
				registers[0xF] = 1;
			}
			else
			{
				registers[0xF] = 0;
			}
		}

		void OP_8XYE()
		{
			unsigned short X = (opcode & 0x0F00) >> 8;
			unsigned short Y = (opcode & 0x00F0) >> 4;
			unsigned char Vx = registers[X];

			registers[X] <<= 1;
			registers[0xF] = Vx >> 7;
		}

		void OP_9XY0()
		{
			unsigned short X = (opcode & 0x0F00) >> 8;
			unsigned short Y = (opcode & 0x00F0) >> 4;

			if (registers[X] != registers[Y])
			{
				program_counter += 2;
			}
		}

		void OP_ANNN()
		{
			unsigned short NNN = opcode & 0x0FFF;

			I = NNN;
		}

		void OP_BNNN()
		{
			unsigned short NNN = opcode & 0x0FFF;

			program_counter = NNN + registers[0];
		}

		void OP_CXNN()
		{
			unsigned char randomNumber = static_cast<unsigned char>(generateRandomNumber(0, 0xFF));
			unsigned char X = (opcode & 0x0F00) >> 8;
			unsigned char NN = opcode & 0x00FF;

			registers[X] = NN & randomNumber;
		}

		void OP_DXYN()
		{
			unsigned char Vx = (opcode & 0x0F00) >> 8;
			unsigned char Vy = (opcode & 0x00F0) >> 4;
			unsigned char N = opcode & 0x000F;

			unsigned char X = registers[Vx] % 64;
			unsigned char Y = registers[Vy] % 32;
			registers[0xF] = 0;

			for (int row = 0;row < N;row++) {
				unsigned char curr_sprite = memory[I + row];

				for (int col = 0;col < 8;col++) {
					unsigned char spriteBit = curr_sprite & (0x80 >> col);
					unsigned char* displayPixel = &display[(Y + row) * DISPLAY_WIDTH + (X + col)];

					if (spriteBit) {
						if (*displayPixel) {
							*displayPixel = 0;
							registers[0xF] = 1;
						}
						else {
							*displayPixel = 1;
						}
					}
				}
			}
		}

		void OP_EX9E() {
			unsigned char X = (opcode & 0x0F00) >> 8;
			unsigned char Vx = registers[X];

			if (keypad[Vx]) {
				program_counter += 2;
			}
		}

		void OP_EXA1() {
			unsigned char X = (opcode & 0x0F00) >> 8;
			unsigned char Vx = registers[X];

			if (!keypad[Vx]) {
				program_counter += 2;
			}
		}

		void OP_FX07() {
			unsigned char X = (opcode & 0x0F00) >> 8;

			registers[X] = delay_timer;
		}

		void OP_FX15() {
			unsigned char X = (opcode & 0x0F00) >> 8;

			delay_timer = registers[X];
		}

		void OP_FX18() {
			unsigned char X = (opcode & 0x0F00) >> 8;

			sound_timer = registers[X];
		}

		void OP_FX1E()
		{
			unsigned char X = (opcode & 0x0F00) >> 8;

			I += registers[X];
		}

		void OP_Fx0A()
		{
			unsigned char X = (opcode & 0x0F00u) >> 8;

			for (int i = 0;i<16;i++)
			{
				if (keypad[i])
				{
					registers[X] = i;
					return;
				}
			}

			program_counter -= 2;
		}

		void OP_FX29()
		{
			unsigned char X = (opcode & 0x0F00u) >> 8;

			I = FONT_START_ADDRESS + 5 * registers[X];
		}

		void OP_FX33()
		{
			unsigned char X = (opcode & 0x0F00u) >> 8;
			unsigned char Vx = registers[X];

			for (int i = 2;i>=0;i--)
			{
				memory[I + i] = Vx % 10;
				Vx /= 10;
			}
		}

		void OP_FX55()
		{
			unsigned char X = (opcode & 0x0F00u) >> 8;

			for (int i = 0;i<=X;i++)
			{
				memory[I + i] = registers[i];
			}
		}

		void OP_FX65()
		{
			unsigned char X = (opcode & 0x0F00u) >> 8;

			for (int i = 0;i <= X;i++)
			{
				registers[i] = memory[I + i];
			}
		}
};




#endif //CHIP8_CPP_CLION_CHIP8_H