#include <cstdint>
#include <string>
#include <random>

struct Chip8{

	uint8_t V[16]{};
	uint8_t memory[4096]{};
	uint16_t index{};
	uint16_t pc{};
	uint16_t stack[16]{};
	uint8_t sp{};
	uint8_t delay_timer{};
	uint8_t sound_timer{};
	uint8_t keypad[16]{};
	uint32_t display[64 * 32]{};
	uint16_t opcode;

	const uint8_t DISPLAY_WIDTH = 64;
	const uint8_t DISPLAY_HEIGHT = 32;

	const uint8_t FONTSET_START_ADDRESS = 0x50;
	const uint8_t PROGRAM_START_ADDRESS = 0x200;

	const uint8_t fontset[80] =
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

	void OP_00E0()
	{
		memset(display, 0, sizeof(display));
	}

	void OP_00EE()
	{
		pc = stack[sp];
		--sp;
	}

	void OP_1nnn()
	{
		pc = opcode & 0x0FFFu;
	}

	void OP_2nnn()
	{
		++sp;
		stack[sp] = pc;
		pc = opcode & 0x0FFFu;
	}

	void OP_3xkk()
	{
		uint8_t Vx = V[(opcode & 0x0F00u) >> 8u];
		uint8_t kk = opcode & 0x00FFu;

		if (Vx == kk)
			pc += 2;
	}

	void OP_4xkk()
	{
		uint8_t Vx = V[(opcode & 0x0F00u) >> 8u];
		uint8_t kk = opcode & 0x00FFu;

		if (Vx != kk)
			pc += 2;
	}

	void OP_5xy0()
	{
		uint8_t Vx = V[(opcode & 0x0F00u) >> 8u];
		uint8_t Vy = V[(opcode & 0x00F0u) >> 4u];

		if (Vx == Vy)
			pc += 2;
	}

	void OP_6xkk()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;
		uint8_t kk = opcode & 0x00FFu;

		V[Vx] = kk;

	}

	void OP_7xkk()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;
		uint8_t kk = opcode & 0x00FFu;

		V[Vx] += kk;
	}

	void OP_8xy0() 
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;
		uint8_t Vy = V[(opcode & 0x00F0u) >> 4u];

		V[Vx] = Vy;

	}

	void OP_8xy1()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;
		uint8_t Vy = (opcode & 0x00F0u) >> 4u;

		V[Vx] = (V[Vx] | V[Vy]);
	}

	void OP_8xy2()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;
		uint8_t Vy = (opcode & 0x00F0u) >> 4u;

		V[Vx] = (V[Vx] & V[Vy]);
	}

	void OP_8xy3()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;
		uint8_t Vy = (opcode & 0x00F0u) >> 4u;

		V[Vx] = (V[Vx] ^ V[Vy]);
	}

	void OP_8xy4()
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

	void OP_8xy5()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;
		uint8_t Vy = (opcode & 0x00F0u) >> 4u;

		if (V[Vx] > V[Vy])
			V[0xF] = 1;
		else
			V[0xF] = 0;

		V[Vx] = V[Vx] - V[Vy];
	}

	void OP_8xy6()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;
		V[0xF] = (V[Vx] & 1u);

		V[Vx] >>= 1;

	}

	void OP_8xy7()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;
		uint8_t Vy = (opcode & 0x00F0u) >> 4u;

		if (V[Vy] > V[Vx])
			V[0xF] = 1;
		else
			V[0xF] = 0;

		V[Vx] = V[Vy] - V[Vx];
	}

	void OP_8xyE()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;
		V[0xF] = (V[Vx] & 0x80u) >> 7u;

		V[Vx] <<= 1;
	}

	void OP_9xy0()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;
		uint8_t Vy = (opcode & 0x00F0u) >> 4u;

		if (V[Vx] != V[Vy])
			pc += 2;
	}

	void OP_Annn()
	{
		index = opcode & 0x0FFFu;
	}

	void OP_Bnnn()
	{
		pc = (opcode & 0x0FFFu) + V[0x0];
	}

	void OP_Cxkk()
	{

		uint8_t Vx = (opcode & 0x0F00u) >> 8u;
		uint8_t kk = opcode & 0x00FFu;

		std::random_device rd;
		std::mt19937 gen(rd);
		std::uniform_int_distribution<unsigned int> distrib(0u, 255u);

		V[Vx] = (distrib(gen) & kk);

	}

	void OP_Dxyn()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;
		uint8_t Vy = (opcode & 0x00F0u) >> 4u;

		uint8_t sprite_length = opcode & 0x000Fu;

		uint8_t x = V[Vx] % DISPLAY_WIDTH;
		uint8_t y = V[Vy] & DISPLAY_HEIGHT;
		
		V[0xF] = 0;

		for (unsigned int row = 0; row < sprite_length; ++row)
		{
			uint8_t sprite = memory[index + row];

			for (unsigned int col; col < 8; ++col)
			{
				uint8_t sprite_pixel = sprite & (0x80u >> col);
				uint32_t* screen_pixel = &display[(y + row) *  + (x + col)];

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
	}

	void OP_Ex9E()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;

		if (keypad[V[Vx]])
			pc += 2;

	}

	void OP_ExA1()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;

		if (!keypad[V[Vx]])
			pc += 2;
	}

	void OP_Fx07()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;
		
		V[Vx] = delay_timer;
	}

	void OP_Fx0A()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;

		for (auto& i : keypad)
		{
			if (i)
			{
				V[Vx] = i;
				return;
			}	
		}

		pc -= 2;
	}

	void OP_Fx15()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;

		delay_timer = V[Vx];

	}

	void OP_Fx18()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;
		sound_timer = V[Vx];
	}

	void OP_Fx1E()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;
		index += V[Vx];
	}

	void OP_Fx29()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;
		uint8_t digit = V[Vx];

		index = FONTSET_START_ADDRESS + (5 * digit);
	}

	void OP_Fx33()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;
		uint8_t value = V[Vx];

		memory[index + 2] = value % 10;
		value /= 10;

		memory[index + 1] = value % 10;
		value /= 10;

		memory[index] = value % 10;
	}

	void OP_Fx55()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;

		for (unsigned int i = 0x0; i < Vx; ++i)
		{
			memory[index + i] = V[i];
		}
	}

	void OP_Fx65()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;

		for (unsigned int i = 0x0; i < Vx; ++i)
		{
			V[i] = memory[index + i];
		}
	}

};