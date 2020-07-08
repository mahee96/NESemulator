#pragma once
#pragma warning( disable : 26812)
#include<cstdint>

enum MIRROR
{
	HARDWARE,
	HORIZONTAL,
	VERTICAL,
	ONESCREEN_LO,
	ONESCREEN_HI,
};

class Mapper
{
protected:
	uint8_t nPRGBanks = 0;
	uint8_t nCHRBanks = 0;

public:
	Mapper(uint8_t prgBanks, uint8_t chrBanks);
	~Mapper();

	//Interfaces to different mapper implementations(pure virtual functions)
	
	//Program data(cpu instructions)
	virtual bool cpuMapRead(const uint16_t addr, uint32_t& mapped_addr, uint8_t &data) = 0;
	virtual bool cpuMapWrite(const uint16_t addr, uint32_t& mapped_addr, uint8_t data = 0) = 0;
	
	//Character data(Graphics data)
	virtual bool gpuMapRead(const uint16_t addr, uint32_t& mapped_addr) = 0;
	virtual bool gpuMapWrite(const uint16_t addr, uint32_t& mapped_addr) = 0;

	//Reset Mapper Circuit to known state
	virtual void reset() = 0;

	// Get MIRROR mode if mapper is in control
	virtual MIRROR mirror();

	// IRQ Interface
	virtual bool irqState();
	virtual void irqClear();

	// Scanline Counting
	virtual void scanline();
};

