#include <stdint.h>
#include <cstdio>

#define STEG_HEADER_SIZE sizeof(uint32_t) * 8

enum ImageType {
	PNG, JPG, BMP, TGA
};

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


};








