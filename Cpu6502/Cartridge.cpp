#include "Cartridge.h"

Cartridge::Cartridge(const std::string& sFileName) {
	struct RomHeader {
		char name[4];
		uint8_t prg_rom_chunks;
		uint8_t chr_rom_chunks;
		uint8_t mapper1;
		uint8_t mapper2;
		uint8_t prg_ram_size;
		uint8_t tv_system1;
		uint8_t tv_system2;
		char reserved[5];
	} header;

	std::ifstream ifs;
	ifs.open(sFileName, std::ifstream::binary);
	if (ifs.is_open()) {
		//read rom header
		ifs.read((char*)&header, sizeof(RomHeader));
		if (header.mapper1 & 0x04)				//training info (unwanted)
			ifs.seekg(512, std::ios_base::cur);	//skip this

		//get mapper Id
		nMapperId = ((header.mapper2 >> 4) << 4) | (header.mapper1 >> 4);
		//get nametable mirroring type
		mirror = (header.mapper1 & 0x01) ? VERTICAL : HORIZONTAL;
		//get iNes File format
		uint8_t nFileType = 1;
		if (nFileType == 0) {}
		else if (nFileType == 1) {
			nPRGBanks = header.prg_rom_chunks;
			vPRGrom.resize((uint16_t)nPRGBanks * 16384);	//n * 1 bank(std) ROM = 16KB 
			ifs.read((char*)vPRGrom.data(), vPRGrom.size());
			
			nCHRBanks = header.chr_rom_chunks;
			vCHRrom.resize(nCHRBanks * 8192);	//n * 1 bank(8KB) ROM = 8KB
			ifs.read((char*)vCHRrom.data(), vCHRrom.size());
		}
		else if (nFileType == 2) {}

		//load correct mapper
		switch (nMapperId)
		{
		case 0: 
			pMapper = std::make_shared<Mapper_000>(nPRGBanks, nCHRBanks);
			break;
		}
		ifs.close();
	}

}

Cartridge::~Cartridge(){
}

bool Cartridge::cpuBusRead(const uint16_t addr, uint8_t&data){
	uint32_t mapped_addr = 0;
	if (pMapper->cpuMapRead(addr, mapped_addr)) {
		data = vPRGrom[mapped_addr];
		return true;
	} 
	return false;					//event handled?
}

bool Cartridge::cpuBusWrite(const uint16_t addr, const uint8_t data){
	uint32_t mapped_addr = 0;
	if (pMapper->cpuMapWrite(addr, mapped_addr)) {
		vPRGrom[mapped_addr] = data;
		return true;
	}
	return false;					//event handled?
}

bool Cartridge::gpuBusRead(const uint16_t addr, uint8_t& data){
	uint32_t mapped_addr = 0;
	if (pMapper->gpuMapRead(addr, mapped_addr)) {
		data = vCHRrom[mapped_addr];
		return true;
	}
	return false;					//event handled?
}

bool Cartridge::gpuBusWrite(const uint16_t addr, const uint8_t data){
	uint32_t mapped_addr = 0;
	if (pMapper->gpuMapWrite(addr, mapped_addr)) {
		vCHRrom[mapped_addr] = data;
		return true;
	}
	return false;					//event handled?
}
