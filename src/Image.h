#include <stdint.h>
#include <cstdio>
#include "schrift.h"

#define STEG_HEADER_SIZE sizeof(uint32_t) * 8

enum ImageType {
	PNG, JPG, BMP, TGA
};

struct Font;


struct Image {
	uint8_t* data = NULL;
	size_t size = 0;
	int w;
	int h;
	int channels;

	Image(const char* filename);
	Image(int w, int h, int channels);
	Image(const Image& img);
	~Image();

	bool read(const char* filename);
	bool write(const char* filename);

	ImageType get_file_type(const char* filename);

	


	


	
	Image& diffmap(Image& img);
	Image& diffmap_scale(Image& img, uint8_t scl = 0);


	Image& grayscale_avg();
	Image& grayscale_lum();

	Image& color_mask(float r, float g, float b);

	
	Image& encodeMessage(const char* message);
	Image& decodeMessage(char* buffer, size_t* messageLength);

	Image& flipX();
	Image& flipY();

	Image& overlay(const Image& source, int x, int y);

	Image& overlayText(const char* txt, const Font& font, int x, int y, uint8_t r = 255, uint8_t g = 255, uint8_t b = 255, uint8_t a = 255);

	Image& crop(uint16_t cx, uint16_t cy, uint16_t cw, uint16_t ch);
};


struct Font {
	SFT sft = {NULL, 12, 12, 0, 0, SFT_DOWNWARD_Y|SFT_RENDER_IMAGE};
	Font(const char* fontfile, uint16_t size) {
		if((sft.font = sft_loadfile(fontfile)) == NULL) {
			printf("\e[31m[ERROR] Failed to load %s\e[0m\n", fontfile);
			return;
		}
		setSize(size);
	} 
	~Font() {
		sft_freefont(sft.font);
	}
	void setSize(uint16_t size) {
		sft.xScale = size;
		sft.yScale = size;
	}
};
