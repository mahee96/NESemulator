#pragma once
#include <cstdint>
#include <string>
#include <fstream>
#include <vector>

#include "Mapper_000.h"
#include "Mapper_001.h"
#include "Mapper_002.h"
#include "Mapper_003.h"
#include "Mapper_004.h"
#include "Mapper_066.h"

class Cartridge
{
private:
	bool bImageValid = false;
	MIRROR hw_mirroring = HORIZONTAL;

	uint8_t nMapperId = 0;
	uint8_t nPRGBanks = 0;
	uint8_t nCHRBanks = 0;

	std::vector<uint8_t> vPRGrom;	//program rom bytes
	std::vector<uint8_t> vCHRrom;	//picture/char rom bytes

	std::shared_ptr<Mapper> pMapper;

public:
	
	Cartridge(const std::string& sFileName);
	~Cartridge();

	// System Bus Read & Write
	bool cpuBusRead(const uint16_t addr, uint8_t& data);
	bool cpuBusWrite(const uint16_t addr, const uint8_t data);
	//Gpu Bus Read & Write
	bool gpuBusRead(const uint16_t addr, uint8_t& data);
	bool gpuBusWrite(const uint16_t addr, const uint8_t data);
	
	//Returns true if the ROM data is valid
	bool ImageValid();

	// Permits system rest of mapper to know state
	void reset();

	// Get Mirror configuration
	MIRROR Mirror();

	std::shared_ptr<Mapper> GetMapper();
};

