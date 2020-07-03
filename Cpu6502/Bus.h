#pragma once
#include <cstdint>
#include <array>
#include <iostream>

#include "Cpu6502.h"
#include "Gpu2C02.h"
#include "Cartridge.h"

class Bus
{
private:
	uint32_t systemClockCounter = 0;
	uint8_t controllerState[2];	//saved state of controllers[when cpu polls for data] 

	uint8_t dma_page = 0x00;
	uint8_t dma_addr = 0x00;
	uint8_t dma_data = 0x00;

	bool dma_transfer = false;
	bool dma_dummy = true;

public:
	// Devices on bus
	Cpu6502 cpu;
	Gpu2C02 gpu;
	uint8_t mController[2];	//instantaneous state of 2 controllers
	std::shared_ptr<Cartridge> cartridge;
	// System Ram - 2 KB
	std::array<uint8_t, 0x2048> systemRAM = { 0 };

	// Fake RAM for this part of the series
	//std::array<uint8_t, 0xFFFF> cpuRam = { 0 };	//full addressable range of CPU

	Bus();
	~Bus();

	// Bus Read & Write
	void cpuBusWrite(const uint16_t addr, const uint8_t data);
	uint8_t cpuBusRead(const uint16_t addr, const bool bReadOnly = false);

	void insertCartridge(const std::shared_ptr<Cartridge>& cartridge);
	void reset();
	void clock();
};

