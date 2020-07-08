#pragma once
#include "Mapper.h"
class Mapper_000 : public Mapper
{
public:
    Mapper_000(uint8_t prgBanks, uint8_t chrBanks);
    ~Mapper_000();

    // Inherited via Mapper
    bool cpuMapRead(const uint16_t addr, uint32_t& mapped_addr, uint8_t &data) override;
    bool cpuMapWrite(const uint16_t addr, uint32_t& mapped_addr, const uint8_t data = 0) override;
    bool gpuMapRead(const uint16_t addr, uint32_t& mapped_addr) override;
    bool gpuMapWrite(const uint16_t addr, uint32_t& mapped_addr) override;
    void reset() override;
};

