
#include <iostream>
#include <sstream>
#include <deque>

#include "Bus.h"
//#include "Cpu6502.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_SOUND
#include "olcPGEX_Sound.h"


class Debugger : public olc::PixelGameEngine
{
public:
	Debugger() { sAppName = "Cpu6502 Debugger"; }

	bool bEmulationRun = false;
	float fResidualTime = 0.0f;
	uint8_t nSelectedPalette = 0x00;

	Bus nes;
	std::shared_ptr<Cartridge> cartridge;

	std::map<uint16_t, std::string> mapAsm;

	std::list<uint16_t> audio[4];
	float fAccumulatedTime = 0.0f;

	std::string hex(uint32_t n, uint8_t d)
	{
		std::string s(d, '0');
		for (int i = d - 1; i >= 0; i--, n >>= 4)
			s[i] = "0123456789ABCDEF"[n & 0xF];
		return s;
	};

	void DrawRam(int x, int y, uint16_t nAddr, int nRows, int nColumns)
	{
		int nRamX = x, nRamY = y;
		for (int row = 0; row < nRows; row++)
		{
			std::string sOffset = "$" + hex(nAddr, 4) + ":";
			for (int col = 0; col < nColumns; col++)
			{
				sOffset += " " + hex(nes.cpuBusRead(nAddr, true), 2);
				nAddr += 1;
			}
			DrawString(nRamX, nRamY, sOffset);
			nRamY += 10;
		}
	}

	void DrawCpu(int x, int y)
	{
		std::string status = "STATUS: ";
		DrawString(x, y, "STATUS:", olc::WHITE);
		
		DrawString(x  + 64,	 y, "N", nes.cpu.status & Cpu6502::N ? olc::GREEN : olc::RED);
		DrawString(x  + 80,  y, "V", nes.cpu.status & Cpu6502::V ? olc::GREEN : olc::RED);
		DrawString(x  + 96,  y, "-", nes.cpu.status & Cpu6502::U ? olc::GREEN : olc::RED);
		DrawString(x  + 112, y, "B", nes.cpu.status & Cpu6502::B ? olc::GREEN : olc::RED);
		DrawString(x  + 128, y, "D", nes.cpu.status & Cpu6502::D ? olc::GREEN : olc::RED);
		DrawString(x  + 144, y, "I", nes.cpu.status & Cpu6502::I ? olc::GREEN : olc::RED);
		DrawString(x  + 160, y, "Z", nes.cpu.status & Cpu6502::Z ? olc::GREEN : olc::RED);
		DrawString(x  + 178, y, "C", nes.cpu.status & Cpu6502::C ? olc::GREEN : olc::RED);
		
		DrawString(x , y + 10, "PC: $" +  hex(nes.cpu.pc, 4));
		DrawString(x , y + 20, "A:  $" +  hex(nes.cpu.a, 2) + "  [" + std::to_string(nes.cpu.a) + "]");
		DrawString(x , y + 30, "X:  $" +  hex(nes.cpu.x, 2) + "  [" + std::to_string(nes.cpu.x) + "]");
		DrawString(x , y + 40, "Y:  $" +  hex(nes.cpu.y, 2) + "  [" + std::to_string(nes.cpu.y) + "]");
		DrawString(x , y + 50, "Stack P: $" + hex(nes.cpu.stkp, 4));
	}

	void DrawCode(int x, int y, int nLines)
	{
		//std::map<uint16_t,std::string>::iterator iterator = mapAsm.find(nes.cpu.pc);
		auto iterator = mapAsm.find(nes.cpu.pc);
		int nLineY = (nLines >> 1) * 10 + y;
		if (iterator != mapAsm.end())
		{
			DrawString(x, nLineY, (*iterator).second, olc::CYAN);
			while (nLineY < (nLines * 10) + y)
			{
				nLineY += 10;
				if (++iterator != mapAsm.end())
				{
					DrawString(x, nLineY, (*iterator).second);
				}
			}
		}

		iterator = mapAsm.find(nes.cpu.pc);
		nLineY = (nLines >> 1) * 10 + y;
		if (iterator != mapAsm.end())
		{
			while (nLineY > y)
			{
				nLineY -= 10;
				if (--iterator != mapAsm.end())
				{
					DrawString(x, nLineY, (*iterator).second);
				}
			}
		}
	}

	void DrawAudio(int channel, int x, int y)
	{
		FillRect(x, y, 120, 120, olc::BLACK);
		int i = 0;
		for (auto s : audio[channel])
		{
			Draw(x + i, y + (s >> (channel == 2 ? 5 : 4)), olc::YELLOW);
			i++;
		}
	}

	// This function is called by the underlying sound hardware
	// which runs in a different thread. It is automatically
	// synchronised with the sample rate of the sound card, and
	// expects a single "sample" to be returned, whcih ultimately
	// makes its way to your speakers, and then your ears, for that
	// lovely 8-bit bliss... but, that means we've some thread
	// handling to deal with, since we want both the PGE thread
	// and the sound system thread to interact with the emulator.

	static Debugger* pInstance; // Static variable that will hold a pointer to "this"

	static float SoundOut(int nChannel, float fGlobalTime, float fTimeStep)
	{
		if (nChannel == 0)
		{
			while (!pInstance->nes.clock()) {};
			return static_cast<float>(pInstance->nes.dAudioSample);
		}
		else
			return 0.0f;
	}

	bool OnUserCreate() override
	{
		// Load Program (assembled at https://www.masswerk.at/6502/assembler.html)
		/*
			*=$8000
			LDX #10
			STX $0000
			LDX #3
			STX $0001
			LDY $0000
			LDA #0
			CLC
			loop
			ADC $0001
			DEY
			BNE loop
			STA $0002
			NOP
			NOP
			NOP
		*/

		// Convert hex string into bytes for RAM
		/*
		std::stringstream ss;
		ss << "A2 0A 8E 00 00 A2 03 8E 01 00 AC 00 00 A9 00 18 6D 01 00 88 D0 FA 8D 02 00 EA EA EA";
		uint16_t nOffset = 0x8000;
		while (!ss.eof())
		{
			std::string b;
			ss >> b;
			nes.cpuRam[nOffset++] = (uint8_t)std::stoul(b, nullptr, 16);
		}
		*/

		// Set Reset IRQ Vector ID
		/*
		nes.cpuRam[0xFFFC] = 0x00;
		nes.cpuRam[0xFFFD] = 0x80;
		*/

		
		//cartridge = std::make_shared<Cartridge>("nestest.nes");
		//cartridge = std::make_shared<Cartridge>("ai4.nes");
		cartridge = std::make_shared<Cartridge>("smb.nes");
		//cartridge = std::make_shared<Cartridge>("dk.nes");
		if (!cartridge->ImageValid())
			return false;
		nes.insertCartridge(cartridge);
		// Dont forget to set IRQ and NMI vectors if you want to play with those
		
		// Extract dissassembly
		//mapAsm = nes.cpu.disassemble(0x0000, 0xFFFF);

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 120; j++)
				audio[i].push_back(0);
		}

		// Reset
		nes.cpu.reset();
		pInstance = this;
		nes.SetSampleFrequency(44100);
		olc::SOUND::InitialiseAudio(44100, 1, 8, 512);
		olc::SOUND::SetUserSynthFunction(SoundOut);
		return true;
	}

	// We must play nicely now with the sound hardware, so unload
	// it when the application terminates
	bool OnUserDestroy() override
	{
		olc::SOUND::DestroyAudio();
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		EmulatorUpdateWithAudio(fElapsedTime);
		return true;
	}
	
	// This performs an emulation update but synced to audio, so it cant
	// perform stepping through code or frames. Essentially, it runs
	// the emulation in real time now, so only accepts "controller" input
	// and updates the display
	bool EmulatorUpdateWithAudio(float fElapsedTime)
	{
		// Sample audio channel output roughly once per frame
		fAccumulatedTime += fElapsedTime;
		if (fAccumulatedTime >= 1.0f / 60.0f)
		{
			fAccumulatedTime -= (1.0f / 60.0f);
			audio[0].pop_front();
			audio[0].push_back(nes.apu.pulse1_visual);
			audio[1].pop_front();
			audio[1].push_back(nes.apu.pulse2_visual);
			audio[2].pop_front();
			audio[2].push_back(nes.apu.noise_visual);
		}

		Clear(olc::DARK_BLUE);	//clear console to blank
		/*
		if (GetKey(olc::Key::SPACE).bPressed)
		{
			do
			{
				nes.cpu.clock();
			} 
			while (!nes.cpu.complete());
		}

		if (GetKey(olc::Key::R).bPressed)
			nes.cpu.reset();

		if (GetKey(olc::Key::I).bPressed)
			nes.cpu.irq();

		if (GetKey(olc::Key::N).bPressed)
			nes.cpu.nmi();
		*/
		// Draw Ram Page 0x00		
		//DrawRam(2, 2, 0x2000, 8*2, 16);
		//DrawRam(2, 182, 0x2500, 8*2, 16);
		//DrawRam(2, 362, 0x3000, 4*2, 16);
		//DrawCpu(448, 2);
		//DrawCode(448, 72, 26);

		//DrawString(10, 370, "SPACE = Step Instruction    R = RESET    I = IRQ    N = NMI");

		nes.mController[0]  = 0x00;
		nes.mController[0] |= GetKey(olc::Key::X).bHeld ? 0x80 : 0x00;
		nes.mController[0] |= GetKey(olc::Key::Z).bHeld ? 0x40 : 0x00;
		nes.mController[0] |= GetKey(olc::Key::A).bHeld ? 0x20 : 0x00;
		nes.mController[0] |= GetKey(olc::Key::S).bHeld ? 0x10 : 0x00;
		nes.mController[0] |= GetKey(olc::Key::UP).bHeld ? 0x08 : 0x00;
		nes.mController[0] |= GetKey(olc::Key::DOWN).bHeld ? 0x04 : 0x00;
		nes.mController[0] |= GetKey(olc::Key::LEFT).bHeld ? 0x02 : 0x00;
		nes.mController[0] |= GetKey(olc::Key::RIGHT).bHeld ? 0x01 : 0x00;

		//if (bEmulationRun) {
		//	if (fResidualTime > 0.0f)
		//		fResidualTime -= fElapsedTime;
		//	else {
		//		fResidualTime += (1.0f / 60.0f) - fElapsedTime;
		//		do { nes.clock(); } while (!nes.gpu.frame_complete);	//if host is faster FPS could spike
		//		nes.gpu.frame_complete = false;
		//	}
		//}
		//else {
		//	if (GetKey(olc::Key::C).bPressed) {	//Handle "C" Key press
		//		//clock enough to execute 1 whole instruction
		//		do { nes.clock(); } while (!nes.cpu.complete());
		//		//since cpu clock is slower than system clock 
		//		//so fush out current system clock cycle if any intermediate.
		//		do { nes.clock(); } while (!nes.cpu.complete());
		//	}

		//	if (GetKey(olc::Key::F).bPressed) {	//Handle "F" Key press
		//		//clock enough to execute 1 whole Frame
		//		do { nes.clock(); } while (!nes.gpu.frame_complete);
		//		//use residual clock cycles to complete current instruction
		//		do { nes.clock(); } while (!nes.gpu.frame_complete);
		//		nes.gpu.frame_complete = false;
		//	}
		//}
		if (GetKey(olc::Key::R).bPressed) {	nes.reset();}							//Handle "C" Key press
		//if (GetKey(olc::Key::SPACE).bPressed) { bEmulationRun = !bEmulationRun; }	//Handle "C" Key press
		if (GetKey(olc::Key::P).bPressed) { ++nSelectedPalette &= 0x07; }			//Handle "P" Key press

		//DrawCpu(516, 2);
		//DrawCode(516, 72, 26);

		/*for (int i = 0; i < 26; i++) {
			std::string s = hex(i, 2) + ": (" + std::to_string(nes.gpu.pOAM[i * 4 + 3])
				+ ", " + std::to_string(nes.gpu.pOAM[i * 4 + 0]) + ")"
				+ " ID: " + hex(nes.gpu.pOAM[i * 4 + 1], 2) +
				+ " AT: " + hex(nes.gpu.pOAM[i * 4 + 2], 2);
			DrawString(516, 72 + i * 10, s); 
		}*/

		//// Draw AUDIO Channels
		//DrawAudio(0, 520, 72);
		//DrawAudio(1, 644, 72);
		//DrawAudio(2, 520, 196);
		//DrawAudio(3, 644, 196);

		//olc::Sprite& s0 = nes.gpu.GetPatternTable(0, nSelectedPalette);
		//olc::Sprite& s1 = nes.gpu.GetPatternTable(1, nSelectedPalette);
		////DrawSprite(516, 72, &s0,2);

		//DrawSprite(516, 348, &s0);
		//DrawSprite(648, 348, &s1);

		//const int nSwatchSize = 6;
		//for (int p = 0; p < 8; p++)		//for each palette
		//	for (int s = 0; s < 4; s++)	//for each index
		//		FillRect(516 + p * (nSwatchSize * 5) + s * nSwatchSize, 340,
		//			nSwatchSize, nSwatchSize, nes.gpu.GetColorFromPaletteRam(p, s));

		//DrawRect(516 + nSelectedPalette * (nSwatchSize * 5) - 1, 339, (nSwatchSize * 4),
		//	nSwatchSize, olc::WHITE);


		DrawSprite(0, 0, &nes.gpu.GetScreen(), 2);
		//for(uint8_t y = 0; y < 30; y++)
		//	for (uint8_t x = 0; x < 32; x++)
		//	{
		//		//DrawString(x * 16, y * 16, hex((uint32_t)nes.gpu.nameTableRAM[0][((uint16_t)y << 5) | x], 2));
		//		//DrawString(x * 16, y * 16, hex((uint32_t)nes.gpu.nameTableRAM[0][y * 32 + x], 2));
		//		uint8_t id = (uint32_t)nes.gpu.nameTableRAM[0][y * 32 + x];
		//		DrawPartialSprite(x * 16, y * 16, &s0,
		//			(id & 0x0F) << 3, ((id >> 4) & 0x0F) << 3, 8, 8, 2);
		//	}

		return true;
	}
};

//our static pointer
Debugger* Debugger::pInstance = nullptr;


int main() {
	Debugger session;
	//session.Construct(780, 480, 2, 2);
	session.Construct(480, 480, 2, 2);
	session.Start();
	return 0;
}
