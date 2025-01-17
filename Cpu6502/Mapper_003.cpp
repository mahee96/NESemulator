#include "Mapper_003.h"

Mapper_003::Mapper_003(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks){}

Mapper_003::~Mapper_003(){}

bool Mapper_003::cpuMapRead(uint16_t addr, uint32_t &mapped_addr, uint8_t &data) {
	if (addr >= 0x8000 && addr <= 0xFFFF) {
		if (nPRGBanks == 1) // 16K ROM 
			mapped_addr = addr & 0x3FFF;
		else if (nPRGBanks == 2) // 32K ROM
			mapped_addr = addr & 0x7FFF;
		return true;
	}
	return false;
}

bool Mapper_003::cpuMapWrite(uint16_t addr, uint32_t &mapped_addr, uint8_t data) {
	if (addr >= 0x8000 && addr <= 0xFFFF) {
		nCHRBankSelect = data & 0x03;
		mapped_addr = addr;		
	}
	return false;	// Mapper has handled write, but do not update ROMs
}

bool Mapper_003::gpuMapRead(uint16_t addr, uint32_t &mapped_addr) {
	if (addr < 0x2000) {
		mapped_addr = nCHRBankSelect * 0x2000 + addr;
		return true;
	}
	return false;
}

bool Mapper_003::gpuMapWrite(uint16_t addr, uint32_t &mapped_addr) { return false; }

void Mapper_003::reset() { nCHRBankSelect = 0; }
