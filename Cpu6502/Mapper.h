#pragma once
#include<cstdint>

class Mapper
{
protected:
	uint8_t nPRGBanks = 0;
	uint8_t nCHRBanks = 0;

public:
	Mapper(uint8_t prgBanks, uint8_t chrBanks);
	~Mapper();

	virtual bool cpuMapRead(const uint16_t addr, uint32_t& mapped_addr) = 0;
	virtual bool cpuMapWrite(const uint16_t addr, uint32_t& mapped_addr) = 0;
	virtual bool gpuMapRead(const uint16_t addr, uint32_t& mapped_addr) = 0;
	virtual bool gpuMapWrite(const uint16_t addr, uint32_t& mapped_addr) = 0;
};

