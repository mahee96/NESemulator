#include "Mapper_000.h"

Mapper_000::Mapper_000(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks){
}

Mapper_000::~Mapper_000(){
}

bool Mapper_000::cpuMapRead(const uint16_t addr, uint32_t& mapped_addr) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        mapped_addr = addr & (nPRGBanks > 1 ? 0x7FFF : 0x3FFF);  //32K(2 Bank) or 16K(1 Bank, 2nd one is mirrored) 
        return true;
    }
    return false;
}

bool Mapper_000::cpuMapWrite(const uint16_t addr, uint32_t& mapped_addr){
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        mapped_addr = addr & (nPRGBanks > 1 ? 0x7FFF : 0x3FFF);  //32K(2 Bank) or 16K(1 Bank, 2nd one is mirrored) 
        return true;
    }
    return false;
}

bool Mapper_000::gpuMapRead(const uint16_t addr, uint32_t& mapped_addr){
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        mapped_addr = addr;
        return true;
    }
    return false;
}

bool Mapper_000::gpuMapWrite(const uint16_t addr, uint32_t& mapped_addr){
    /*if (addr >= 0x0000 && addr <= 0x1FFF) {
        return true;
    }*/
    return false;
}