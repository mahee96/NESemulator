#pragma once
#include "Mapper.h"

class Mapper_003 : public Mapper
{
private:
	uint8_t nCHRBankSelect = 0x00;
public:
	Mapper_003(uint8_t prgBanks, uint8_t chrBanks);
	~Mapper_003();

	bool cpuMapRead(uint16_t addr, uint32_t& mapped_addr, uint8_t& data) override;
	bool cpuMapWrite(uint16_t addr, uint32_t& mapped_addr, uint8_t data = 0) override;
	bool gpuMapRead(uint16_t addr, uint32_t& mapped_addr) override;
	bool gpuMapWrite(uint16_t addr, uint32_t& mapped_addr) override;
	void reset() override;
};

