#pragma once
#include <cstdint>
#include <memory>

#include "Cartridge.h"
#include "olcPixelGameEngine.h"

class Gpu2C02
{
private:
	std::shared_ptr<Cartridge> cartridge;			//local ref (weak ptr?)

	uint8_t address_latch = 0x00;
	uint8_t gpu_data_buffer = 0x00;
	uint16_t gpu_address = 0x0000;

	int16_t scanline = 0;	//row of screen
	int16_t cycle = 0;		//col of screen
	
	bool bSpriteZeroHitPossible = false;
	bool bSpriteZeroBeingRendered = false;

	union {
		struct {
			uint8_t reserved : 5;			//0-4 bits reserved
			uint8_t sprite_overflow : 1;
			uint8_t sprite_zero_hit : 1;
			uint8_t vertical_blank : 1;
		};
		uint8_t reg = 0x00;
	} status;								//status of rendering

	union {
		struct {
			uint8_t grayscale : 1;
			uint8_t render_background_left : 1;
			uint8_t render_sprites_left : 1;
			uint8_t render_background : 1;
			uint8_t render_sprites : 1;
			uint8_t enhance_red : 1;
			uint8_t enhance_green : 1;
			uint8_t enhance_blue : 1;
		};
		uint8_t reg = 0x00;
	} mask;

	union GpuCtrl {
		struct {
			uint8_t nametable_x : 1;
			uint8_t nametable_y : 1;
			uint8_t increment_mode : 1;
			uint8_t pattern_sprite : 1;
			uint8_t pattern_background : 1;
			uint8_t sprite_size : 1;
			uint8_t slave_mode : 1;			//unused
			uint8_t enable_nmi : 1;
		};
		uint8_t reg = 0x00;
	} control;

	union GpuRegister {
		struct
		{
			uint16_t coarse_x : 5;
			uint16_t coarse_y : 5;
			uint16_t nametable_x : 1;
			uint16_t nametable_y : 1;
			uint16_t fine_y : 3;
			uint16_t unused : 1;
		};
		uint16_t reg = 0x0000;
	};

	GpuRegister vram_addr;
	GpuRegister addr_buff;

	uint8_t fine_x = 0x00;

	uint8_t bg_next_tile_id = 0x00;
	uint8_t bg_next_tile_attrib = 0x00;
	uint8_t bg_next_tile_lsb = 0x00;
	uint8_t bg_next_tile_msb = 0x00;

	uint16_t bg_shifter_pattern_lo = 0x0000;
	uint16_t bg_shifter_pattern_hi = 0x0000;
	uint16_t bg_shifter_attrib_lo = 0x0000;
	uint16_t bg_shifter_attrib_hi = 0x0000;

	struct sObjectAttributeEntry {	//4 bytes per Sprite(foreground objects)
		uint8_t y;					//Sprite y offset
		uint8_t id;					//Tile(Sprite) id from pattern memory
		uint8_t attribute;			//Sprite attribs
		uint8_t x;					//Sprite x offset
	} OAM[64];						// 4 * 64 = 256 bytes[1 page OAM - Gpu internal RAM] 

	//debugging framebuffer and color pallete vRAM
	olc::Pixel	paletteScreen[0x40];
	olc::Sprite spriteScreen = olc::Sprite(256, 240);
	olc::Sprite spriteNameTable[2] = { olc::Sprite(256,240),olc::Sprite(256,240) };
	olc::Sprite spritePatternTable[2] = { olc::Sprite(128,128),olc::Sprite(128,128) };

public:
	uint8_t nameTableRAM[2][1048] = { {0},{0} };		//nametable Ram 2*1K
	uint8_t patternRAM[2][4096] = { {0},{0} };		//split up into 2 segments/blocks is add-on here
	uint8_t paletteRAM[32] = { 0 };					//color pallete

	uint8_t* pOAM = (uint8_t*)OAM;
	uint8_t oam_addr = 0x00;

	sObjectAttributeEntry spriteScanline[8];
	uint8_t sprite_count;
	uint8_t sprite_shifter_pattern_lo[8];
	uint8_t sprite_shifter_pattern_hi[8];

	bool mFrame_complete = false;
	bool mNmi = false;

	Gpu2C02();
	~Gpu2C02();

	// System Bus Read & Write
	uint8_t cpuBusRead(const uint16_t addr, const bool bReadOnly = false);
	void cpuBusWrite(const uint16_t addr, const uint8_t data);
	//Private bus Read & Write
	uint8_t gpuBusRead(const uint16_t addr, const bool bReadOnly = false);
	void gpuBusWrite(const uint16_t addr, const uint8_t data);

	//debugging framebuffer and color pallete vRAM
	olc::Sprite& GetScreen();
	olc::Sprite& GetNameTable(const uint8_t i);
	olc::Sprite& GetPatternTable(const uint8_t i, const uint8_t palette);
	olc::Pixel&  GetColorFromPaletteRam(const uint8_t palette, const uint8_t pixel);
	
	void connectCartridge(const std::shared_ptr<Cartridge>& cartridge);
	void clock();
};

