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
	
	bImageValid = false;

	std::ifstream ifs;
	ifs.open(sFileName, std::ifstream::binary);
	if (ifs.is_open()) {
		//read rom header
		ifs.read((char*)&header, sizeof(RomHeader));
		if (header.mapper1 & 0x04)				//training info (unwanted)
			ifs.seekg(512, std::ios_base::cur);	//skip this

		//get mapper Id
		nMapperId = ((header.mapper2 >> 4) << 4) | (header.mapper1 >> 4);
		//get nametable MIRRORing type
		hw_mirroring = (header.mapper1 & 0x01) ? VERTICAL : HORIZONTAL;
		
		//get iNes File format
		uint8_t nFileType = 1;
		if ((header.mapper2 & 0x0C) == 0x08) nFileType = 2;

		switch (nFileType)
		{
		case 1:		//Type 1 cartridge
			nPRGBanks = header.prg_rom_chunks;
			vPRGrom.resize((uint16_t)nPRGBanks * 16384);	//n * 1 bank(std) ROM = 16KB 
			ifs.read((char*)vPRGrom.data(), vPRGrom.size());

			nCHRBanks = header.chr_rom_chunks;
			vCHRrom.resize((nCHRBanks == 0 ? 1 : nCHRBanks) * 8192);	//n * 1 bank(8KB) ROM = 8KB
			ifs.read((char*)vCHRrom.data(), vCHRrom.size());
			break;
		case 2:		//Type 2 cartridge
			nPRGBanks = ((header.prg_ram_size & 0x07) << 8) | header.prg_rom_chunks;
			vPRGrom.resize(nPRGBanks * 16384);
			ifs.read((char*)vPRGrom.data(), vPRGrom.size());

			nCHRBanks = ((header.prg_ram_size & 0x38) << 8) | header.chr_rom_chunks;
			vCHRrom.resize(nCHRBanks * 8192);
			ifs.read((char*)vCHRrom.data(), vCHRrom.size());
			break;
		}
		
		//load correct mapper
		switch (nMapperId)
		{
		case  0: pMapper = std::make_shared<Mapper_000>(nPRGBanks, nCHRBanks); break;
		case  1: pMapper = std::make_shared<Mapper_001>(nPRGBanks, nCHRBanks); break;
		case  2: pMapper = std::make_shared<Mapper_002>(nPRGBanks, nCHRBanks); break;
		case  3: pMapper = std::make_shared<Mapper_003>(nPRGBanks, nCHRBanks); break;
		case  4: pMapper = std::make_shared<Mapper_004>(nPRGBanks, nCHRBanks); break;
		case 66: pMapper = std::make_shared<Mapper_066>(nPRGBanks, nCHRBanks); break;
		}
		ifs.close();
		bImageValid = true;	//ROM image is now validated
	}
}

Cartridge::~Cartridge(){}

bool Cartridge::ImageValid(){ return bImageValid; }

bool Cartridge::cpuBusRead(const uint16_t addr, uint8_t& data){
	uint32_t mapped_addr = 0;
	if (pMapper->cpuMapRead(addr, mapped_addr, data)) {		//event handled?
		//if data is populated by mapper then mapped_addr = 0xFFFFFFFF(-1)
		if(mapped_addr != uint32_t(-1))	
			//mapper did't populate data so set it using the offset(into program) provided by mapper
			data = vPRGrom[mapped_addr];	 	
		return true;
	} 
	return false;					//report event not handled
}

bool Cartridge::cpuBusWrite(const uint16_t addr, const uint8_t data){
	uint32_t mapped_addr = 0;
	if (pMapper->cpuMapWrite(addr, mapped_addr)) {	//event handled?
		//if data is used by mapper then mapped_addr = 0xFFFFFFFF(-1)
		if (mapped_addr != uint32_t(-1))
			//mapper did't use data so use it here, using the offset(into program) provided by mapper
			vPRGrom[mapped_addr] = data;
		return true;
	}
	return false;					//report event not handled
}

bool Cartridge::gpuBusRead(const uint16_t addr, uint8_t& data){
	uint32_t mapped_addr = 0;
	if (pMapper->gpuMapRead(addr, mapped_addr)) {	//event handled?
		data = vCHRrom[mapped_addr];
		return true;
	}
	return false;					//report event not handled
}

bool Cartridge::gpuBusWrite(const uint16_t addr, const uint8_t data){
	uint32_t mapped_addr = 0;
	if (pMapper->gpuMapWrite(addr, mapped_addr)) {	//event handled?
		vCHRrom[mapped_addr] = data;
		return true;
	}
	return false;					//report event not handled
}

void Cartridge::reset(){	
	// Resets cartridge circuit and not the program/character ROM
	if (pMapper != nullptr) pMapper->reset();
}

MIRROR Cartridge::Mirror(){
	MIRROR mirroring = pMapper->mirror();
	if (mirroring == MIRROR::HARDWARE) 
		return hw_mirroring;	//Mirroring is hard wired into circuit
	else return mirroring;		//software defined/re-configurable mirroring
}

std::shared_ptr<Mapper> Cartridge::GetMapper(){ return pMapper; }
