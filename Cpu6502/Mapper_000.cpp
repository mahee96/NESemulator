#include "Mapper_000.h"

Mapper_000::Mapper_000(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks){}

Mapper_000::~Mapper_000(){}

void Mapper_000::reset(){}

bool Mapper_000::cpuMapRead(const uint16_t addr, uint32_t& mapped_addr, uint8_t &data) {
    /*
    if PRGROM is 16KB
         CPU Address Bus          PRG ROM
         0x8000 -> 0xBFFF: Map    0x0000 -> 0x3FFF
         0xC000 -> 0xFFFF: Mirror 0x0000 -> 0x3FFF
     if PRGROM is 32KB
         CPU Address Bus          PRG ROM
         0x8000 -> 0xFFFF: Map    0x0000 -> 0x7FFF
    */
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        mapped_addr = addr & (nPRGBanks > 1 ? 0x7FFF : 0x3FFF);  //32K(2 Bank) or 16K(1 Bank, 2nd one is MIRRORed) 
        return true;
    }
    return false;
}

bool Mapper_000::cpuMapWrite(const uint16_t addr, uint32_t& mapped_addr, uint8_t data){
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        mapped_addr = addr & (nPRGBanks > 1 ? 0x7FFF : 0x3FFF);  //32K(2 Bank) or 16K(1 Bank, 2nd one is MIRRORed) 
        return true;
    }
    return false;
}

bool Mapper_000::gpuMapRead(const uint16_t addr, uint32_t& mapped_addr){
    /*
     There is no mapping required for GPU
     GPU Address Bus          CHR ROM
     0x0000 -> 0x1FFF: Map    0x0000 -> 0x1FFF
    */
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        mapped_addr = addr;
        return true;
    }
    return false;
}

bool Mapper_000::gpuMapWrite(const uint16_t addr, uint32_t& mapped_addr){
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        if (nCHRBanks == 0) {
            mapped_addr = addr;
            return true;
        }
    }
    return false;
}
