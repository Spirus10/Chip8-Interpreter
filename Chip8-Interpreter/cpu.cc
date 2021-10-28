#include <cstdint>
#include <fstream>
#include <cstring>
#include <random>
#include <iostream>
#include <vector>

#include "cpu.hh"

Chip8::Chip8() : V(), memory(), index(), pc(), stack(), sp(), delay_timer(), sound_timer(), keypad(), opcode(), display(), drawFlag(false)
{	

}

void Chip8::init()
{
	// Load fontset into main memory
	for (unsigned int i = 0; i < sizeof(fontset); ++i)
	{
		memory[FONTSET_START_ADDRESS + (unsigned int)i] = fontset[i];
	}

	pc = PROGRAM_START_ADDRESS; // Set program counter to 0x200 - the default start address for Chip8 programs
}

void Chip8::load_rom(const char* filename)
{
	std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		std::ifstream::pos_type size = file.tellg();
		std::vector<uint8_t> buffer(size);
		file.seekg(0, std::ios::beg);
		file.read(reinterpret_cast<char*>(buffer.data()), size);
		file.close();

		std::copy(buffer.begin(), buffer.end(), memory.begin() + 512);

		for (unsigned int i = 0; i < size; ++i)
		{
			std::cout << std::hex << "0x" << (PROGRAM_START_ADDRESS + i) << ": ";
			std::cout << memory[PROGRAM_START_ADDRESS + i] << std::endl;
		}

	}
}

void Chip8::exec_instruction()
{
	//std::cout << std::hex << opcode << std::endl;

	switch ((opcode & 0xF000u) >> 12)
	{
	case 0x0:
		switch (opcode & 0x00FFu)
		{
		case 0xE0:
 			OP_00E0();
			return;
		case 0xEE:
			OP_00EE();
			return;
		default:
			OP_null();
			return;
		}

	case 0x1:
		OP_1nnn();
		break;
	case 0x2:
		OP_2nnn();
		break;
	case 0x3:
		OP_3xkk();
		break;
	case 0x4:
		OP_4xkk();
		break;
	case 0x5:
		OP_5xy0();
		break;
	case 0x6:
		OP_6xkk();
		break;
	case 0x7:
		OP_7xkk();
		break;
	case 0x8:
		switch (opcode & 0x000Fu)
		{
		case 0x0:
			OP_8xy0();
			return;
		case 0x1:
			OP_8xy1();
			return;
		case 0x2:
			OP_8xy2();
			return;
		case 0x3:
			OP_8xy3();
			return;
		case 0x4:
			OP_8xy4();
			return;
		case 0x5:
			OP_8xy5();
			return;
		case 0x6:
			OP_8xy6();
			return;
		case 0x7:
			OP_8xy7();
			return;
		case 0xE:
			OP_8xyE();
			return;
		default:
			OP_null();
			return;
		}
	case 0x9:
		OP_9xy0();
		break;
	case 0xA:
		OP_Annn();
		break;
	case 0xB:
		OP_Bnnn();
		break;
	case 0xC:
		OP_Cxkk();
		break;
	case 0xD:
		OP_Dxyn();
		break;
	case 0xE:
		switch (opcode & 0x000Fu)
		{
		case 0x1u:
			OP_ExA1();
			return;
		case 0xEu:
			OP_Ex9E();
			return;
		default:
			OP_null();
			return;
		}
	case 0xF:
		switch (opcode & 0x00FFu)
		{
		case 0x07u:
			OP_Fx07();
			return;
		case 0x0Au:
			OP_Fx0A();
			return;
		case 0x15u:
			OP_Fx15();
			return;
		case 0x18u:
			OP_Fx18();
			return;
		case 0x1Eu:
			OP_Fx1E();
			return;
		case 0x29u:
			OP_Fx29();
			return;
		case 0x33u:
			OP_Fx33();
			return;
		case 0x55u:
			OP_Fx55();
			return;
		case 0x65u:
			OP_Fx65();
			return;
		default:
			OP_null();
			return;
		}
	default:
		OP_null();
		break;
	}
}

void Chip8::cycle()
{
	// Fetch instruction
	opcode = memory[pc] << 8u | memory[pc + 1];

	// Increment program counter
	pc += 2;

	// Call corresponding function from opcode table
	this->exec_instruction();

	// Decrement sound and delay timers if they're greater than 0
	if (delay_timer > 0)
		--delay_timer;

	if (sound_timer > 0)
		--sound_timer;


}

void Chip8::draw(Uint32* pixels)
{
	//for (unsigned int pos = 0; pos < DISPLAY_WIDTH * DISPLAY_HEIGHT; ++pos)
	//{
	//	pixels[pos] = 0xFFFFFF * ((display[pos / 8] >> (7 - pos % 8)) & 1);
	//}
	auto displaySize = sizeof(display) / sizeof(display[0]);

	for (int i = 0; i < displaySize; ++i)
	{
		pixels[i] = display[i];
	}

	drawFlag = false;
}

void Chip8::OP_null()
{
	std::cout << "Opcode not recognized" << std::endl;
}

void Chip8::OP_00E0()
{
	memset(display, 0, sizeof(display));
}

void Chip8::OP_00EE()
{
	--sp;
	pc = stack[sp];
}

void Chip8::OP_1nnn()
{
	uint16_t addr = opcode & 0x0FFFu;
	pc = addr;
}

void Chip8::OP_2nnn()
{
	uint16_t addr = opcode & 0x0FFFu;

	stack[sp] = pc;
	++sp;

	pc = addr;
}

void Chip8::OP_3xkk()
{
	uint8_t Vx = V[(opcode & 0x0F00u) >> 8u];
	uint8_t kk = opcode & 0x00FFu;

	if (Vx == kk)
		pc += 2;
}

void Chip8::OP_4xkk()
{
	uint8_t Vx = V[(opcode & 0x0F00u) >> 8u];
	uint8_t kk = opcode & 0x00FFu;

	if (Vx != kk)
		pc += 2;
}

void Chip8::OP_5xy0()
{
	uint8_t Vx = V[(opcode & 0x0F00u) >> 8u];
	uint8_t Vy = V[(opcode & 0x00F0u) >> 4u];

	if (Vx == Vy)
		pc += 2;
}

void Chip8::OP_6xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t kk = opcode & 0x00FFu;

	V[Vx] = kk;

}

void Chip8::OP_7xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t kk = opcode & 0x00FFu;

	V[Vx] += kk;
}

void Chip8::OP_8xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = V[(opcode & 0x00F0u) >> 4u];

	V[Vx] = Vy;

}

void Chip8::OP_8xy1()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	V[Vx] |= V[Vy];
}

void Chip8::OP_8xy2()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	V[Vx] &= V[Vy];
}

void Chip8::OP_8xy3()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	V[Vx] ^= V[Vy];
}

void Chip8::OP_8xy4()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	uint16_t Vsum = V[Vx] + V[Vy];

	if (Vsum > 255u)
		V[0xF] = 1;
	else
		V[0xF] = 0;

	V[Vx] = (Vsum & 0xFFu);
}

void Chip8::OP_8xy5()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (V[Vx] > V[Vy])
		V[0xF] = 1;
	else
		V[0xF] = 0;

	V[Vx] -= V[Vy];
}

void Chip8::OP_8xy6()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	V[0xF] = (V[Vx] & 0x1u);

	V[Vx] >>= 1;

}

void Chip8::OP_8xy7()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (V[Vy] > V[Vx])
		V[0xF] = 1;
	else
		V[0xF] = 0;

	V[Vx] = V[Vy] - V[Vx];
}

void Chip8::OP_8xyE()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	V[0xF] = (V[Vx] & 0x80u) >> 7u;

	V[Vx] <<= 1;
}

void Chip8::OP_9xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (V[Vx] != V[Vy])
		pc += 2;
}

void Chip8::OP_Annn()
{
	uint16_t addr = opcode & 0x0FFFu;
	index = addr;
}

void Chip8::OP_Bnnn()
{
	uint16_t addr = opcode & 0x0FFFu;
	pc = addr + V[0x0];
}

void Chip8::OP_Cxkk()
{

	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t kk = opcode & 0x00FFu;

	std::mt19937 gen(std::random_device{}());
	std::uniform_int_distribution<unsigned int> distrib(0u, 255u);

	V[Vx] = (distrib(gen) & kk);

}

void Chip8::OP_Dxyn()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	uint8_t sprite_length = opcode & 0x000Fu;

	uint8_t x = V[Vx] % DISPLAY_WIDTH;
	uint8_t y = V[Vy] % DISPLAY_HEIGHT;

	V[0xF] = 0;

	for (unsigned int row = 0; row < sprite_length; ++row)
	{
		uint8_t sprite = memory[index + row];

		for (unsigned int col = 0; col < 8; ++col)
		{
			uint8_t sprite_pixel = sprite & (0x80u >> col);
			uint32_t* screen_pixel = &display[(y + row) * DISPLAY_WIDTH + (x + col)];

			if (sprite_pixel)
			{
				if (*screen_pixel == 0xFFFFFFFF)
				{
					V[0xF] = 1;
				}

				*screen_pixel ^= 0xFFFFFFFF;
			}
		}
	}

	drawFlag = true;
}

void Chip8::OP_Ex9E()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	if (keypad[V[Vx]])
		pc += 2;

}

void Chip8::OP_ExA1()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	if (!keypad[V[Vx]])
		pc += 2;
}

void Chip8::OP_Fx07()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	V[Vx] = delay_timer;
}

void Chip8::OP_Fx0A()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	if (keypad[0])
		V[Vx] = keypad[0];
	if (keypad[1])
		V[Vx] = keypad[1];
	if (keypad[2])
		V[Vx] = keypad[2];
	if (keypad[3])
		V[Vx] = keypad[3];
	if (keypad[4])
		V[Vx] = keypad[4];
	if (keypad[5])
		V[Vx] = keypad[5];
	if (keypad[6])
		V[Vx] = keypad[6];
	if (keypad[7])
		V[Vx] = keypad[7];
	if (keypad[8])
		V[Vx] = keypad[8];
	if (keypad[9])
		V[Vx] = keypad[9];
	if (keypad[10])
		V[Vx] = keypad[10];
	if (keypad[11])
		V[Vx] = keypad[11];
	if (keypad[12])
		V[Vx] = keypad[12];
	if (keypad[13])
		V[Vx] = keypad[13];
	if (keypad[14])
		V[Vx] = keypad[14];
	if (keypad[15])
		V[Vx] = keypad[15];


	pc -= 2;
}

void Chip8::OP_Fx15()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	delay_timer = V[Vx];

}

void Chip8::OP_Fx18()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	sound_timer = V[Vx];
}

void Chip8::OP_Fx1E()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	index += V[Vx];
}

void Chip8::OP_Fx29()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t digit = V[Vx];

	index = FONTSET_START_ADDRESS + (5 * digit);
}

void Chip8::OP_Fx33()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t value = V[Vx];

	memory[index + 2] = value % 10;
	value /= 10;

	memory[index + 1] = value % 10;
	value /= 10;

	memory[index] = value % 10;
}

void Chip8::OP_Fx55()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	for (uint8_t i = 0; i <= Vx; ++i)
	{
		memory[index + i] = V[i];
	}
}

void Chip8::OP_Fx65()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	for (uint8_t i = 0; i < Vx; ++i)
	{
		V[i] = memory[index + i];
	}
}
