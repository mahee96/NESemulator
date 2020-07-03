#include "Gpu2C02.h"

Gpu2C02::Gpu2C02(){
	paletteScreen[0x00] = olc::Pixel(84, 84, 84);
	paletteScreen[0x01] = olc::Pixel(0, 30, 116);
	paletteScreen[0x02] = olc::Pixel(8, 16, 144);
	paletteScreen[0x03] = olc::Pixel(48, 0, 136);
	paletteScreen[0x04] = olc::Pixel(68, 0, 100);
	paletteScreen[0x05] = olc::Pixel(92, 0, 48);
	paletteScreen[0x06] = olc::Pixel(84, 4, 0);
	paletteScreen[0x07] = olc::Pixel(60, 24, 0);
	paletteScreen[0x08] = olc::Pixel(32, 42, 0);
	paletteScreen[0x09] = olc::Pixel(8, 58, 0);
	paletteScreen[0x0A] = olc::Pixel(0, 64, 0);
	paletteScreen[0x0B] = olc::Pixel(0, 60, 0);
	paletteScreen[0x0C] = olc::Pixel(0, 50, 60);
	paletteScreen[0x0D] = olc::Pixel(0, 0, 0);
	paletteScreen[0x0E] = olc::Pixel(0, 0, 0);
	paletteScreen[0x0F] = olc::Pixel(0, 0, 0);

	paletteScreen[0x10] = olc::Pixel(152, 150, 152);
	paletteScreen[0x11] = olc::Pixel(8, 76, 196);
	paletteScreen[0x12] = olc::Pixel(48, 50, 236);
	paletteScreen[0x13] = olc::Pixel(92, 30, 228);
	paletteScreen[0x14] = olc::Pixel(136, 20, 176);
	paletteScreen[0x15] = olc::Pixel(160, 20, 100);
	paletteScreen[0x16] = olc::Pixel(152, 34, 32);
	paletteScreen[0x17] = olc::Pixel(120, 60, 0);
	paletteScreen[0x18] = olc::Pixel(84, 90, 0);
	paletteScreen[0x19] = olc::Pixel(40, 114, 0);
	paletteScreen[0x1A] = olc::Pixel(8, 124, 0);
	paletteScreen[0x1B] = olc::Pixel(0, 118, 40);
	paletteScreen[0x1C] = olc::Pixel(0, 102, 120);
	paletteScreen[0x1D] = olc::Pixel(0, 0, 0);
	paletteScreen[0x1E] = olc::Pixel(0, 0, 0);
	paletteScreen[0x1F] = olc::Pixel(0, 0, 0);

	paletteScreen[0x20] = olc::Pixel(236, 238, 236);
	paletteScreen[0x21] = olc::Pixel(76, 154, 236);
	paletteScreen[0x22] = olc::Pixel(120, 124, 236);
	paletteScreen[0x23] = olc::Pixel(176, 98, 236);
	paletteScreen[0x24] = olc::Pixel(228, 84, 236);
	paletteScreen[0x25] = olc::Pixel(236, 88, 180);
	paletteScreen[0x26] = olc::Pixel(236, 106, 100);
	paletteScreen[0x27] = olc::Pixel(212, 136, 32);
	paletteScreen[0x28] = olc::Pixel(160, 170, 0);
	paletteScreen[0x29] = olc::Pixel(116, 196, 0);
	paletteScreen[0x2A] = olc::Pixel(76, 208, 32);
	paletteScreen[0x2B] = olc::Pixel(56, 204, 108);
	paletteScreen[0x2C] = olc::Pixel(56, 180, 204);
	paletteScreen[0x2D] = olc::Pixel(60, 60, 60);
	paletteScreen[0x2E] = olc::Pixel(0, 0, 0);
	paletteScreen[0x2F] = olc::Pixel(0, 0, 0);

	paletteScreen[0x30] = olc::Pixel(236, 238, 236);
	paletteScreen[0x31] = olc::Pixel(168, 204, 236);
	paletteScreen[0x32] = olc::Pixel(188, 188, 236);
	paletteScreen[0x33] = olc::Pixel(212, 178, 236);
	paletteScreen[0x34] = olc::Pixel(236, 174, 236);
	paletteScreen[0x35] = olc::Pixel(236, 174, 212);
	paletteScreen[0x36] = olc::Pixel(236, 180, 176);
	paletteScreen[0x37] = olc::Pixel(228, 196, 144);
	paletteScreen[0x38] = olc::Pixel(204, 210, 120);
	paletteScreen[0x39] = olc::Pixel(180, 222, 120);
	paletteScreen[0x3A] = olc::Pixel(168, 226, 144);
	paletteScreen[0x3B] = olc::Pixel(152, 226, 180);
	paletteScreen[0x3C] = olc::Pixel(160, 214, 228);
	paletteScreen[0x3D] = olc::Pixel(160, 162, 160);
	paletteScreen[0x3E] = olc::Pixel(0, 0, 0);
	paletteScreen[0x3F] = olc::Pixel(0, 0, 0);
}

Gpu2C02::~Gpu2C02(){
}

uint8_t Gpu2C02::cpuBusRead(const uint16_t addr, const bool bReadOnly){
	uint8_t data = 0x00;
	switch (addr)
	{
	case 0x0000:	//Control
	case 0x0001:	//Mask
		break;
	case 0x0002:	//Status
		data = (status.reg & 0xE0) | (gpu_data_buffer & 0x1F);
		status.vertical_blank = 0;
		address_latch = 0;
		break;
	case 0x0003:	//OAM addr (mapper)
		break;
	case 0x0004:	//OAM data (mapper)
		data = pOAM[oam_addr];
		break;
	case 0x0005:	//Scroll 
		break;
	case 0x0006:	//Gpu Addr 
		break;
	case 0x0007:	//Gpu Data 
		data = gpu_data_buffer;								//read buffered frame
		gpu_data_buffer = gpuBusRead(vram_addr.reg);			//buffer 1 frame for next cycle
		if (vram_addr.reg > 0x3F00) data = gpu_data_buffer;		//use realtime(no buffering) if palette
		//gpu_address++;	//always linear increments (full horizontal row then next row) 
		//for vertical orientation (horizontal full row is not reqd)
		vram_addr.reg += (control.increment_mode) ? 32 : 1;
		break;
	}
	return data;
}

void Gpu2C02::cpuBusWrite(const uint16_t addr, const uint8_t data) {
	switch (addr)
	{
	case 0x0000:	//Control
		control.reg = data;
		addr_buff.nametable_x = control.nametable_x;
		addr_buff.nametable_y = control.nametable_y;
		break;
	case 0x0001:	//Mask
		mask.reg = data;
		break;
	case 0x0002:	//Status
		break;
	case 0x0003:	//OAM addr (mapper)
		oam_addr = data;
		break;
	case 0x0004:	//OAM data (mapper) [never used mostly - DMA is used instead]
		pOAM[oam_addr] = data;
		break;
	case 0x0005:	//Scroll 
		if (address_latch == 0) {
			fine_x = data & 0x07;
			addr_buff.coarse_x = data >> 3;
			address_latch = 1;
		} 
		else {
			addr_buff.fine_y = data & 0x07;
			addr_buff.coarse_y = data >> 3;
			address_latch = 0;
		}
		break;
	case 0x0006:	//Gpu Addr 
		if (address_latch == 0) {
			addr_buff.reg = (addr_buff.reg & 0x00FF) | (uint16_t)((data & 0x3F) << 8);
			address_latch = 1;
		}
		else {
			addr_buff.reg = (addr_buff.reg & 0xFF00) | data;
			vram_addr = addr_buff;
			address_latch = 0;
		}
		break;
	case 0x0007:	//Gpu Data 
		gpuBusWrite(vram_addr.reg, data);
		//gpu_address++;	//always linear increments (full horizontal row then next row) 
		//for vertical orientation (horizontal full row is not reqd)
		vram_addr.reg += (control.increment_mode) ? 32 : 1;
		break;
	}
}

uint8_t Gpu2C02::gpuBusRead(uint16_t addr, const bool bReadOnly){
	uint8_t data = 0x00;
	addr &= 0x3FFF;
	if (cartridge->gpuBusRead(addr, data)) {
	}
	else if (addr >= 0x0000 && addr <= 0x1FFF) {	//pattern Ram
		data = patternRAM[(addr & 0x1000) >> 12][addr & 0x0FFF];
	}
	else if (addr >= 0x2000 && addr <= 0x3EFF) {	//nameTable Ram
		addr &= 0x0FFF;
		if (cartridge->mirror == Cartridge::Mirror::VERTICAL) {
			//Vertical
			if (addr >= 0x0000 && addr <= 0x03FF)
				data = nameTableRAM[0][addr & 0x03FF];
			if (addr >= 0x0400 && addr <= 0x07FF)
				data = nameTableRAM[1][addr & 0x03FF];
			if (addr >= 0x0800 && addr <= 0x0BFF)
				data = nameTableRAM[0][addr & 0x03FF];
			if (addr >= 0x0C00 && addr <= 0x0FFF)
				data = nameTableRAM[1][addr & 0x03FF];
		}
		else if (cartridge->mirror == Cartridge::Mirror::HORIZONTAL) {
			//Horizontal
			if (addr >= 0x0000 && addr <= 0x03FF)
				data = nameTableRAM[0][addr & 0x03FF];
			if (addr >= 0x0400 && addr <= 0x07FF)
				data = nameTableRAM[0][addr & 0x03FF];
			if (addr >= 0x0800 && addr <= 0x0BFF)
				data = nameTableRAM[1][addr & 0x03FF];
			if (addr >= 0x0C00 && addr <= 0x0FFF)
				data = nameTableRAM[1][addr & 0x03FF];
		}
	}
	else if (addr >= 0x3F00 && addr <= 0x3FFF) {	//palette Ram
		addr &= 0x001F;								//mask first 11 bits
		//some mirroring
		if (addr == 0x0010) addr = 0x0000;
		if (addr == 0x0014) addr = 0x0004;
		if (addr == 0x0018) addr = 0x0008;
		if (addr == 0x001C) addr = 0x000C;
		data = paletteRAM[addr];//& (mask.grayscale ? 0x30 : 0x3F);					//get color info from pallete ram
	}
	else throw new std::exception;
	return data;
}

void Gpu2C02::gpuBusWrite(uint16_t addr, const uint8_t data) {
	addr &= 0x3FFF;									
	if (cartridge->gpuBusWrite(addr, data)) {
	} 
	else if (addr >= 0x0000 && addr <= 0x1FFF) {	//pattern Ram
		patternRAM[(addr & 0x1000) >> 12][addr & 0x0FFF] = data;
	} 
	else if (addr >= 0x2000 && addr <= 0x3EFF) {	//nameTable Ram
		addr &= 0x0FFF;
		if (cartridge->mirror == Cartridge::Mirror::VERTICAL) {
			//Vertical
			if (addr >= 0x0000 && addr <= 0x03FF)
				nameTableRAM[0][addr & 0x03FF] = data;
			if (addr >= 0x0400 && addr <= 0x07FF)
				nameTableRAM[1][addr & 0x03FF] = data;
			if (addr >= 0x0800 && addr <= 0x0BFF)
				nameTableRAM[0][addr & 0x03FF] = data;
			if (addr >= 0x0C00 && addr <= 0x0FFF)
				nameTableRAM[1][addr & 0x03FF] = data;
		}
		else if (cartridge->mirror == Cartridge::Mirror::HORIZONTAL) {
			//Horizontal
			if (addr >= 0x0000 && addr <= 0x03FF)
				nameTableRAM[0][addr & 0x03FF] = data;
			if (addr >= 0x0400 && addr <= 0x07FF)
				nameTableRAM[0][addr & 0x03FF] = data;
			if (addr >= 0x0800 && addr <= 0x0BFF)
				nameTableRAM[1][addr & 0x03FF] = data;
			if (addr >= 0x0C00 && addr <= 0x0FFF)
				nameTableRAM[1][addr & 0x03FF] = data;
		}
	} 
	else if (addr >= 0x3F00 && addr <= 0x3FFF) {	//palette Ram
		addr &= 0x001F;								//mask first 11 bits
		//some mirroring
		if (addr == 0x0010) addr = 0x0000;
		if (addr == 0x0014) addr = 0x0004;
		if (addr == 0x0018) addr = 0x0008;
		if (addr == 0x001C) addr = 0x000C;			
		paletteRAM[addr] = data;					//save color info into pallete ram
	}
	else throw new std::exception;
}

void Gpu2C02::connectCartridge(const std::shared_ptr<Cartridge>& cartridge){
	this->cartridge = cartridge;
}

void Gpu2C02::clock(){

	auto IncrementScrollX = [&]() {
	if (mask.render_background || mask.render_sprites) {	//[if Renderer enabled]
		if (vram_addr.coarse_x == 31) {						//curr x offset  == max[32]?
			vram_addr.coarse_x = 0;							//reset x coarse for next NameTable
			vram_addr.nametable_x = ~vram_addr.nametable_x;	//flip bit to use other table
		}
		else vram_addr.coarse_x++;							//cur x offset++(px/tile horizontal);
	}
	};

	auto IncrementScrollY = [&]() {
	if (mask.render_background || mask.render_sprites) {	//[if Renderer enabled]
		if (vram_addr.fine_y < 7)							//1 tile = 8px/bits[0-7]
			vram_addr.fine_y++;								//curr y offset++ (px vertical)
		else {
			vram_addr.fine_y = 0;							//reset y offset
			if (vram_addr.coarse_y == 29) {					//curr y offset == max[30]?
				vram_addr.coarse_y = 0;						//reset y coarse for next NameTable
				vram_addr.nametable_y = ~vram_addr.nametable_y;	//flip bit to use other table
			}
			else if (vram_addr.coarse_y == 31)				//curr y coarse == extndd max[32]? [attrib]
				vram_addr.coarse_y = 0;						//reset y coarse
			else vram_addr.coarse_y++;						//curr y offset++ (tile vertical)
		}
	}
	};

	auto TransferAddressX = [&]() {
	if (mask.render_background || mask.render_sprites) {	//[if Renderer enabled]
		//gpu idle to process new data set by cpu
		//receive new frame data(NameTable) updated by cpu, set in addr_buff
		vram_addr.nametable_x = addr_buff.nametable_x;		//get new NameTable x offset
		vram_addr.coarse_x = addr_buff.coarse_x;			//get new x coarse(tile) offset
	}
	};

	auto TransferAddressY = [&]() {
	if (mask.render_background || mask.render_sprites) {	//[if Renderer enabled]
		vram_addr.nametable_y = addr_buff.nametable_y;		//get new NameTable y offset
		vram_addr.coarse_y = addr_buff.coarse_y;			//get new y coarse(tile) offset
		vram_addr.fine_y = addr_buff.fine_y;				//get new y fine(px in tile) offset
	}
	};

	auto LoadBackgroundShifters = [&]()
	{
	bg_shifter_pattern_lo = (bg_shifter_pattern_lo & 0xFF00) | bg_next_tile_lsb;
	bg_shifter_pattern_hi = (bg_shifter_pattern_hi & 0xFF00) | bg_next_tile_msb;
	bg_shifter_attrib_lo = (bg_shifter_attrib_lo & 0xFF00) | ((bg_next_tile_attrib & 0b01) ? 0xFF : 0x00);
	bg_shifter_attrib_hi = (bg_shifter_attrib_hi & 0xFF00) | ((bg_next_tile_attrib & 0b10) ? 0xFF : 0x00);
	};

	auto UpdateShifters = [&]() {
		if (mask.render_background) {
			bg_shifter_pattern_lo <<= 1;
			bg_shifter_pattern_hi <<= 1;

			bg_shifter_attrib_lo <<= 1;
			bg_shifter_attrib_hi <<= 1;
		}

		if (mask.render_sprites && cycle >= 1 && cycle < 258) {
			for (int i = 0; i < sprite_count; i++) {
				if (spriteScanline[i].x > 0) spriteScanline[i].x--;
				else {
					sprite_shifter_pattern_lo[i] <<= 1;
					sprite_shifter_pattern_hi[i] <<= 1;
				}
			}
		}

	};

	if (scanline >= -1 && scanline < 240){
		// Background Rendering ======================================================
		if (scanline == 0 && cycle == 0) {
			// "Odd Frame" cycle skip
			cycle = 1;
		}
		if (scanline == -1 && cycle == 1) {
			status.vertical_blank = 0;
			status.sprite_zero_hit = 0;
			status.sprite_overflow = 0;

			for (int i = 0; i < 8; i++) {
				//clear out shifters (pixel is transparent and no rendering is reqd)
				sprite_shifter_pattern_lo[i] = 0;
				sprite_shifter_pattern_hi[i] = 0;
			}
		}
		if ((cycle >= 2 && cycle < 258) || (cycle >= 321 && cycle < 338)) {
			UpdateShifters();
			switch ((cycle - 1) % 8)
			{
			case 0:
				LoadBackgroundShifters();
				bg_next_tile_id = gpuBusRead(0x2000 | (vram_addr.reg & 0x0FFF));
				break;
			case 2:
				bg_next_tile_attrib = gpuBusRead((0x2000 + 0x03C0)
					| (vram_addr.nametable_y << 11)
					| (vram_addr.nametable_x << 10)
					| ((vram_addr.coarse_y >> 2) << 3)
					| (vram_addr.coarse_x >> 2));
				if (vram_addr.coarse_y & 0x02) bg_next_tile_attrib >>= 4;
				if (vram_addr.coarse_x & 0x02) bg_next_tile_attrib >>= 2;
				bg_next_tile_attrib &= 0x03;
				break;
			case 4:
				bg_next_tile_lsb = gpuBusRead((control.pattern_background << 12)
					+ ((uint16_t)bg_next_tile_id << 4)
					+ (vram_addr.fine_y) + 0);
				break;
			case 6:
				bg_next_tile_msb = gpuBusRead((control.pattern_background << 12)
					+ ((uint16_t)bg_next_tile_id << 4)
					+ (vram_addr.fine_y) + 8);
				break;
			case 7:
				IncrementScrollX();
				break;
			}
		}
		if (cycle == 256) IncrementScrollY();
		if (cycle == 257) {
			LoadBackgroundShifters();
			TransferAddressX();
		}
		if (cycle == 338 || cycle == 340) {
			bg_next_tile_id = gpuBusRead(0x2000 | (vram_addr.reg & 0x0FFF));
		}
		if (scanline == -1 && cycle >= 280 && cycle < 305) TransferAddressY();

		//Foreground rendering:
		//Sprite(candidate) evaluation for rendering
		if (cycle == 257 && scanline >= 0) {
			std::memset(spriteScanline, 0xFF, 8 * sizeof(sObjectAttributeEntry));
			sprite_count = 0;
			// Secondly, clear out any residual information in sprite pattern shifters
			for (uint8_t i = 0; i < 8; i++)
			{
				sprite_shifter_pattern_lo[i] = 0;
				sprite_shifter_pattern_hi[i] = 0;
			}

			uint8_t nOAMEntry = 0;
			bSpriteZeroHitPossible = false;
			while (nOAMEntry < 64 && sprite_count < 9) {								//if total OAM objects and sprite count within range
				int16_t diff = ((int16_t)scanline - (int16_t)OAM[nOAMEntry].y);	//sprite y offset with scanline 
				if (diff >= 0 && diff < (control.sprite_size ? 16 : 8)) {			//is the spreite(8x8 or 16x8) interacting with scanline
					if (sprite_count < 8) {											//total of sprites within max range?
						if (nOAMEntry == 0) bSpriteZeroHitPossible = true;
						memcpy(&spriteScanline[sprite_count], &OAM[nOAMEntry], sizeof(sObjectAttributeEntry));	//copy sprite from OAM
						sprite_count++;												//no of sprites in use++
					}
				}
				nOAMEntry++;														//try next tile of sprite/next sprite in OAM
			}
			status.sprite_overflow = (sprite_count > 8);							//if requested sprites exceeded range? set overflow flag
		}
		//Actual Sprite data setup for rendering
		if (cycle == 340) {
			for (uint8_t i = 0; i < sprite_count; i++) {
				uint8_t sprite_pattern_bits_lo, sprite_pattern_bits_hi;
				uint16_t sprite_pattern_addr_lo, sprite_pattern_addr_hi;	//used to read(uint16_t) char/pattern mem(gpuBusRead)

				if (!control.sprite_size) {	//sprite size 8x8?
					//8x8
					if (!(spriteScanline[i].attribute & 0x80)) {	//vertically normal?
						sprite_pattern_addr_lo =
							(control.pattern_sprite << 12)			//pattern table id(page 1(0x0000) or page 2(0x4096))
							| (spriteScanline[i].id << 4)			//Tile ID(x) [from 16x16 tiles of 4K sized pattern table]
							| (scanline - spriteScanline[i].y);	//row offset(y)

					}
					else {											//vertically flipped!
						sprite_pattern_addr_lo =
							(control.pattern_sprite << 12)			//pattern table id(page 1(0x0000) or page 2(0x4096))
							| (spriteScanline[i].id << 4)			//Tile ID(x) [from 16x16 tiles of 4K sized pattern table]
							| (7 - (scanline - spriteScanline[i].y));	//row offset(y) in the flipped/inverted tile
					}
				}
				else {		//sprite size 8x16
					//8x16
					if (!(spriteScanline[i].attribute & 0x80)) {				//vertically normal?
						if (scanline - spriteScanline[i].y < 8) {				//top 8x8 in 8x16 tile 	
							sprite_pattern_addr_lo =
								((spriteScanline[i].id & 0x01) << 12)			//which pattern table == LSB of chosen Tile ID
								| ((spriteScanline[i].id & 0xFE) << 4)			//read only top 8x8 tile from 0-8(tileY)
								| ((scanline - spriteScanline[i].y) & 0x07);	//read row offsetted by y in chosen 8x8 tile

						}
						else {													//bottom 8x8 in 8x16 tile
							sprite_pattern_addr_lo =
								((spriteScanline[i].id & 0x01) << 12)			//which pattern table == LSB of chosen Tile ID
								| (((spriteScanline[i].id & 0xFE) + 1) << 4)	//read only bottom 8x8 tile from 8-16(tileY)
								| ((scanline - spriteScanline[i].y) & 0x07);	//read row offsetted by y in chosen 8x8 tile
						}
					}
					else {															//vertically flipped/inverted!
						if (scanline - spriteScanline[i].y < 8) {					//top 8x8 in 8x16 tile
							sprite_pattern_addr_lo =
								((spriteScanline[i].id & 0x01) << 12)				//which pattern table == LSB of chosen Tile ID
								| (((spriteScanline[i].id & 0xFE) + 1) << 4)			//read only top 8x8 tile from 0-8(tileY)
								| (7 - (scanline - spriteScanline[i].y) & 0x07);	//read row offsetted by y in chosen 8x8 tile

						}
						else {														//bottom 8x8 in 8x16 tile
							sprite_pattern_addr_lo =
								((spriteScanline[i].id & 0x01) << 12)					//which pattern table == LSB of chosen Tile ID
								| ((spriteScanline[i].id & 0xFE) << 4)				//read only bottom 8x8 tile from 8-16(tileY)
								| (7 - (scanline - spriteScanline[i].y) & 0x07);	//read row offsetted by y in chosen 8x8 tile
						}
					}
				}

				sprite_pattern_addr_hi = sprite_pattern_addr_lo + 8;	//hi addr is tile pair offset by 8 bytes

				sprite_pattern_bits_lo = gpuBusRead(sprite_pattern_addr_lo);	//get the pattern data at 1st tile in tile pair
				sprite_pattern_bits_hi = gpuBusRead(sprite_pattern_addr_hi);	//get the pattern data at 2nd tile in tile pair

				if (spriteScanline[i].attribute & 0x40) {
					//flip the binary data horizontally (used in image processing for mirroring bitmaps)
					auto flipbyte = [](uint8_t b) {
						b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
						b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
						b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
						return b;
					};
					//flip bitmaps horizontally(mirror images)
					sprite_pattern_bits_lo = flipbyte(sprite_pattern_bits_lo);
					sprite_pattern_bits_hi = flipbyte(sprite_pattern_bits_hi);
				}

				//load sprite data into shift registers like in bg_tiles (movement/scrolling)
				sprite_shifter_pattern_lo[i] = sprite_pattern_bits_lo;
				sprite_shifter_pattern_hi[i] = sprite_pattern_bits_hi;
			}
		}
	}

	if (scanline == 240) { /* Post Render ScanLine - Do Nothing */ }

	if (scanline >= 241 && scanline < 261){
		if (scanline == 241 && cycle == 1) {
			status.vertical_blank = 1;
			if (control.enable_nmi)	mNmi = true;
		}
	}

	uint8_t bg_pixel = 0x00;
	uint8_t bg_palette = 0x00;
	if (mask.render_background) {
		uint16_t bit_mux = 0x8000 >> fine_x;

		uint8_t p0_pixel = (bg_shifter_pattern_lo & bit_mux) > 0;
		uint8_t p1_pixel = (bg_shifter_pattern_hi & bit_mux) > 0;
		bg_pixel = (p1_pixel << 1) | p0_pixel;

		uint8_t bg_pal0 = (bg_shifter_attrib_lo & bit_mux) > 0;
		uint8_t bg_pal1 = (bg_shifter_attrib_hi & bit_mux) > 0;
		bg_palette = (bg_pal1 << 1) | bg_pal0;
	}

	uint8_t fg_pixel = 0x00;
	uint8_t fg_palette = 0x00;
	uint8_t fg_priority = 0x00;
	if (mask.render_sprites) {
		bSpriteZeroBeingRendered = false;
		for (uint8_t i = 0; i < sprite_count; i++) {
			if (spriteScanline[i].x == 0) {		//start of sprite
				uint8_t fg_pixel_lo = (sprite_shifter_pattern_lo[i] & 0x80) > 0;
				uint8_t fg_pixel_hi = (sprite_shifter_pattern_hi[i] & 0x80) > 0;
				fg_pixel = (fg_pixel_hi << 1) | (fg_pixel_lo);	//2 bit fg pixel (bitmap in nes)
				fg_palette = (spriteScanline[i].attribute & 0x03) + 0x04;	//2 bit fg palette (color id)
				fg_priority = (spriteScanline[i].attribute & 0x20) == 0;	//can the hide behind background?

				if (fg_pixel != 0) {
					if (i == 0) bSpriteZeroBeingRendered = true;
					break;	//OAM/Sprite objects with least rank have higher priority and
							//if non-transparent break and use it!
				}

			}
		}
	}

	uint8_t pixel = 0x00;	//final pixel displayed
	uint8_t palette = 0x00; //final color of displayed pixel

	if (bg_pixel == 0 && fg_pixel == 0) {		//both transprent
		pixel = 0x00;	 //keep transparent
		palette = 0x00;  //use transparent color
	}
	else if (bg_pixel == 0 && fg_pixel > 0) {	//fg_pixel chosen
		pixel = fg_pixel;		//use fg
		palette = fg_palette;	//use fg
	}
	else if (bg_pixel > 0 && fg_pixel == 0) {		//bg_pixel chosen
		pixel = bg_pixel;		//use bg
		palette = bg_palette;	//use bg
	}
	else if (bg_pixel > 0 && fg_pixel > 0) {		//fg_pixel && bg_pixel chosen
		if (fg_priority) {
			pixel = fg_pixel;		//use fg
			palette = fg_palette;	//use fg
		}
		else {
			pixel = bg_pixel;		//use bg
			palette = bg_palette;	//use bg
		}
		if (bSpriteZeroHitPossible && bSpriteZeroBeingRendered) {
			if (mask.render_background && mask.render_sprites) {
				if (~(mask.render_background_left | mask.render_sprites_left)) {
					if (cycle >= 9 && cycle < 258) status.sprite_zero_hit = 1;
				}
				else {
					if(cycle >=1 && cycle < 258) status.sprite_zero_hit = 1;
				}
			}
		}
	}

	spriteScreen.SetPixel(cycle - 1, scanline, GetColorFromPaletteRam(palette, pixel));

	//Generate random Noise to look like TV without reception
	//spriteScreen.SetPixel(cycle - 1, scanline, paletteScreen[(rand() % 2) ? 0x3F : 0x30]);

	//GPU renderer internal clock (never stops painting screen) - check TV scanning for more details
	cycle++;

	if (cycle >= 341) {					//cols done?
		cycle = 0;						//reset col/freq Counter
		scanline++;					//set next row(increment) scan
		if (scanline >= 261) {			//rows done?
			scanline = -1;				//reset to top (for next frame)
			mFrame_complete = true;		//mark current frame, Complete!
		}
	}
}

olc::Sprite& Gpu2C02::GetScreen(){ return this->spriteScreen; }

olc::Sprite& Gpu2C02::GetNameTable(const uint8_t i){
	return spriteNameTable[i];
}

olc::Sprite& Gpu2C02::GetPatternTable(const uint8_t i, const uint8_t palette){
	for (uint16_t nTileY = 0; nTileY < 16; nTileY++) {
		for (uint16_t nTileX = 0; nTileX < 16; nTileX++) {
			
			const uint16_t nOffset = (nTileY * 256) + (nTileX * 16);
			for (uint16_t row = 0; row < 8; row++) {
				uint8_t tile_lsb = gpuBusRead(i * 0x1000 + nOffset + row + 0);
				uint8_t tile_msb = gpuBusRead(i * 0x1000 + nOffset + row + 8);

				for (uint16_t col = 0; col < 8; col++) {
					const uint8_t pixel = ((tile_lsb & 0x01) << 1)| (tile_msb & 0x01);
					tile_lsb >>= 1; tile_msb >>= 1;

					spritePatternTable[i].SetPixel(
						nTileX * 8 + (7 - col),
						nTileY * 8 + row,
						GetColorFromPaletteRam(palette, pixel)
					);
				}
			}
		}
	}
	return spritePatternTable[i];
}

olc::Pixel& Gpu2C02::GetColorFromPaletteRam(const uint8_t palette, const uint8_t pixel){
	return paletteScreen[gpuBusRead(0x3F00 + (palette << 2) + pixel) & 0x3F];
}