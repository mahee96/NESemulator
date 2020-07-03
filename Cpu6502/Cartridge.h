#pragma once
#include <climits>
#include <cstdint>
#include <fstream>
#include <vector>

#include "Mapper_000.h"

class Cartridge
{
private:
	uint8_t nMapperId = 0;
	uint8_t nPRGBanks = 0;
	uint8_t nCHRBanks = 0;

	std::vector<uint8_t> vPRGrom;	//program rom bytes
	std::vector<uint8_t> vCHRrom;	//picture/char rom bytes

	std::shared_ptr<Mapper> pMapper;

public:
	enum Mirror {
		HORIZONTAL,
		VERTICAL,
		ONESCREEN_LO,
		ONESCREEN_HI
	} mirror = HORIZONTAL;

	Cartridge(const std::string& sFileName);
	~Cartridge();

	// System Bus Read & Write
	bool cpuBusRead(const uint16_t addr, uint8_t& data);
	bool cpuBusWrite(const uint16_t addr, const uint8_t data);
	//Gpu Bus Read & Write
	bool gpuBusRead(const uint16_t addr, uint8_t& data);
	bool gpuBusWrite(const uint16_t addr, const uint8_t data);

};

