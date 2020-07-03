#include "Bus.h"

Bus::Bus(){
	// Connect CPU to System bus
	cpu.ConnectBus(this);
	
	// Clear RAM contents, just in case :P
	std::fill(systemRAM.begin(), systemRAM.end(), 0x00);
	//for (auto &i : ram) i = 0x00;
}

Bus::~Bus(){}

void Bus::cpuBusWrite(const uint16_t addr, const uint8_t data){
	if (cartridge->cpuBusWrite(addr, data)) return;		//event handled? then exit
	else if (addr >= 0x0000 && addr <= 0x1FFF)				//8KB System RAM ?
		systemRAM[addr & 0x7FF] = data;						//masking to same loc
	else if (addr >= 0x2000 && addr <= 0x3FFF)				//gpu RAMs?
		gpu.cpuBusWrite(addr & 0x0007, data);				//write gpu regs
	else if (addr == 0x4014) {								//DMA reg for CPU's ram to GPU's OAM transfer 
		dma_page = data;
		dma_addr = 0x00;
		dma_transfer = true;
	}
	else if (addr >= 0x4016 && addr <= 0x4017)				//controllers?
		controllerState[addr & 0x0001] = mController[addr & 0x0001]; //save state
}

uint8_t Bus::cpuBusRead(const uint16_t addr, const bool bReadOnly) {
	uint8_t data = 0x00;
	if (cartridge->cpuBusRead(addr, data)) {}				//event handled? then exit
	else if (addr >= 0x0000 && addr <= 0x1FFF) 				//8KB System RAM
		data = systemRAM[addr & 0x7FF];						//masking to same loc
	else if (addr >= 0x2000 && addr <= 0x3FFF) 				//gpu RAMs?
		data = gpu.cpuBusRead(addr & 0x0007, bReadOnly);	//read gpu regs
	else if (addr >= 0x4016 && addr <= 0x4017){				//controllers?
		data = (controllerState[addr & 0x0001] & 0x80) > 0; //save state into a shift reg
		controllerState[addr & 0x0001] <<= 1;	
	}
	return data;
}

void Bus::insertCartridge(const std::shared_ptr<Cartridge>& cartridge){
	this->cartridge = cartridge;
	gpu.connectCartridge(cartridge);
}

void Bus::reset(){
	cpu.reset();										//reset cpu state
	systemClockCounter = 0;							//reset System Bus clock
}

void Bus::clock(){
	gpu.clock();										//gpu clock cycle++
	if (systemClockCounter % 3 == 0) {					//cpu cycle = gpu_cycle/3 (3 times slower) 
		if (dma_transfer) {								//DMA request on 0x4014? skip cpu clock++ (suspended)
			if (dma_dummy) {							//(DMA)cleanup idle time enabled after completion?
				if (systemClockCounter % 2 == 1) {		//only on odd clock of system bus/cpu
					dma_dummy = false;					//reset the cleanup idle time
				}
			}
			else {		//for 256 bytes of 1 page size DMA 
				if (systemClockCounter % 2 == 0) {		//even clock cycles
					dma_data = cpuBusRead(dma_page << 8 | dma_addr);	//(Read RAM data into DMA_buffer)
				}
				else {
					gpu.pOAM[dma_addr] = dma_data;		//odd clock cycles(Write DMA_buffer into OAM memory)
					dma_addr++;							//DMA R/W addr ptr++
					if (dma_addr == 0x00) {				//addr wrap over?
						dma_transfer = false;			//transfer complete
						dma_dummy = true;				//Set cleanup idle time
					}
				}
			}
		}
		else {
			cpu.clock();								//cpu clock is 3 times slower than gpu clock
		}
	}
	if (gpu.mNmi) {									//gpu signalled mNmi for cpu?
		gpu.mNmi = false;								//reset state
		cpu.nmi();										//percolate signal to cpu mNmi pin
	}

	systemClockCounter++;								//System Bus clock cycle++
}

