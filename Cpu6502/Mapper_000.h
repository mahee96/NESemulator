#pragma once
#include "Mapper.h"
class Mapper_000 : public Mapper
{
public:
    Mapper_000(uint8_t prgBanks, uint8_t chrBanks);
    ~Mapper_000();

    // Inherited via Mapper
    virtual bool cpuMapRead(const uint16_t addr, uint32_t& mapped_addr) override;
    virtual bool cpuMapWrite(const uint16_t addr, uint32_t& mapped_addr) override;
    virtual bool gpuMapRead(const uint16_t addr, uint32_t& mapped_addr) override;
    virtual bool gpuMapWrite(const uint16_t addr, uint32_t& mapped_addr) override;
};

