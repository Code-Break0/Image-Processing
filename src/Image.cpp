#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define BYTE_BOUND(value) value < 0 ? 0 : (value > 255 ? 255 : value)
#include "Image.h"
#include "stb_image.h"
#include "stb_image_write.h"



Image::Image(const char* filename) {
	if(read(filename)) {
		printf("Read %s\n", filename);
		size = w*h*channels;
	}
	else {
		printf("Failed to read %s\n", filename);
	}
}

Image::Image(int w, int h, int channels) : w(w), h(h), channels(channels) {
	size = w*h*channels;
	data = new uint8_t[size];
}

Image::Image(const Image& img) : Image(img.w, img.h, img.channels) {
	memcpy(data, img.data, size);
}

Image::~Image() {
	stbi_image_free(data);
}

bool Image::read(const char* filename) {
	data = stbi_load(filename, &w, &h, &channels, 0);
	return data != NULL;
}

bool Image::write(const char* filename) {
	ImageType type = get_file_type(filename);
	int success;
  switch (type) {
    case PNG:
      success = stbi_write_png(filename, w, h, channels, data, w*channels);
      break;
    case BMP:
      success = stbi_write_bmp(filename, w, h, channels, data);
      break;
    case JPG:
      success = stbi_write_jpg(filename, w, h, channels, data, 100);
      break;
    case TGA:
      success = stbi_write_tga(filename, w, h, channels, data);
      break;
  }
  return success != 0;
}

ImageType Image::get_file_type(const char* filename) {
	const char* ext = strrchr(filename, '.');
	if(ext != nullptr) {
		if(strcmp(ext, ".png") == 0) {
			return PNG;
		}
		else if(strcmp(ext, ".jpg") == 0) {
			return JPG;
		}
		else if(strcmp(ext, ".bmp") == 0) {
			return BMP;
		}
		else if(strcmp(ext, ".tga") == 0) {
			return TGA;
		}
	}
	return PNG;
}








Image& Image::diffmap(Image& img) {
	int compare_width = fmin(w,img.w);
	int compare_height = fmin(h,img.h);
	int compare_channels = fmin(channels,img.channels);
	for(uint32_t i=0; i<compare_height; ++i) {
		for(uint32_t j=0; j<compare_width; ++j) {
			for(uint8_t k=0; k<compare_channels; ++k) {
				data[(i*w+j)*channels+k] = BYTE_BOUND(abs(data[(i*w+j)*channels+k] - img.data[(i*img.w+j)*img.channels+k]));
			}
		}
	}
	return *this;
}
Image& Image::diffmap_scale(Image& img, uint8_t scl) {
	int compare_width = fmin(w,img.w);
	int compare_height = fmin(h,img.h);
	int compare_channels = fmin(channels,img.channels);
	uint8_t largest = 0;
	for(uint32_t i=0; i<compare_height; ++i) {
		for(uint32_t j=0; j<compare_width; ++j) {
			for(uint8_t k=0; k<compare_channels; ++k) {
				data[(i*w+j)*channels+k] = BYTE_BOUND(abs(data[(i*w+j)*channels+k] - img.data[(i*img.w+j)*img.channels+k]));
				largest = fmax(largest, data[(i*w+j)*channels+k]);
			}
		}
	}
	scl = 255/fmax(1, fmax(scl, largest));
	for(int i=0; i<size; ++i) {
		data[i] *= scl;
	}
	return *this;
}


Image& Image::grayscale_avg() {
	if(channels < 3) {
		printf("Image %p has less than 3 channels, it is assumed to already be grayscale.", this);
	}
	else {
		for(int i = 0; i < size; i+=channels) {
			//(r+g+b)/3
			int gray = (data[i] + data[i+1] + data[i+2])/3;
			memset(data+i, gray, 3);
		}
	}
	return *this;
}


Image& Image::grayscale_lum() {
	if(channels < 3) {
		printf("Image %p has less than 3 channels, it is assumed to already be grayscale.", this);
	}
	else {
		for(int i = 0; i < size; i+=channels) {
			int gray = 0.2126*data[i] + 0.7152*data[i+1] + 0.0722*data[i+2];
			memset(data+i, gray, 3);
		}
	}
	return *this;
}


Image& Image::color_mask(float r, float g, float b) {
	if(channels < 3) {
		printf("\e[31m[ERROR] Color mask requires at least 3 channels, but this image has %d channels\e[0m\n", channels);
	}
	else {
		for(int i = 0;i < size;i+=channels) {
			data[i] *= r;
			data[i+1] *= g;
			data[i+2] *= b;
		}
	}
	return *this;
}




Image& Image::encodeMessage(const char* message) {
	uint32_t len = strlen(message) * 8;
	if(len + STEG_HEADER_SIZE > size) {
		printf("\e[31m[ERROR] This message is too large (%lu bits / %zu bits)\e[0m\n", len+STEG_HEADER_SIZE, size);
		return *this;
	}

	for(uint8_t i = 0;i < STEG_HEADER_SIZE;++i) {
		data[i] &= 0xFE;
		data[i] |= (len >> (STEG_HEADER_SIZE - 1 - i)) & 1UL;
	}

	for(uint32_t i = 0;i < len;++i) {
		data[i+STEG_HEADER_SIZE] &= 0xFE;
		data[i+STEG_HEADER_SIZE] |= (message[i/8] >> ((len-1-i)%8)) & 1;
	}

	return *this;
}

Image& Image::decodeMessage(char* buffer, size_t* messageLength) {
	uint32_t len = 0;
	for(uint8_t i = 0;i < STEG_HEADER_SIZE;++i) {
		len = (len << 1) | (data[i] & 1);
	}
	*messageLength = len / 8;

	for(uint32_t i = 0;i < len;++i) {
		buffer[i/8] = (buffer[i/8] << 1) | (data[i+STEG_HEADER_SIZE] & 1);
	}


	return *this;
}












	Image& Image::flipX() {
		uint8_t tmp[4];
		uint8_t* px1;
		uint8_t* px2;
		for(int y = 0;y < h;++y) {
			for(int x = 0;x < w/2;++x) {
				px1 = &data[(x + y * w) * channels];
				px2 = &data[((w - 1 - x) + y * w) * channels];

				memcpy(tmp, px1, channels);
				memcpy(px1, px2, channels);
				memcpy(px2, tmp, channels);
			}
		}
		return *this;
	}
	Image& Image::flipY() {
		uint8_t tmp[4];
		uint8_t* px1;
		uint8_t* px2;
		for(int x = 0;x < w;++x) {
			for(int y = 0;y < h/2;++y) {
				px1 = &data[(x + y * w) * channels];
				px2 = &data[(x + (h - 1 - y) * w) * channels];

				memcpy(tmp, px1, channels);
				memcpy(px1, px2, channels);
				memcpy(px2, tmp, channels);
			}
		}
		return *this;
	}








Image& Image::overlay(const Image& source, int x, int y) {

	uint8_t* srcPx;
	uint8_t* dstPx;

	for(int sy = 0;sy < source.h;++sy) {
		if(sy + y < 0) {continue;}
		else if(sy + y >= h) {break;}
		for(int sx = 0;sx < source.w;++sx) {
			if(sx + x < 0) {continue;}
			else if(sx + x >= w) {break;}
			srcPx = &source.data[(sx + sy * source.w) * source.channels];
			dstPx = &data[(sx + x + (sy + y) * w) * channels];

			float srcAlpha = source.channels < 4 ? 1 : srcPx[3] / 255.f;
			float dstAlpha = channels < 4 ? 1 : dstPx[3] / 255.f;

			if(srcAlpha > .99 && dstAlpha > .99) {
				if(source.channels >= channels) {
					memcpy(dstPx, srcPx, channels);
				}
				else {
					// In case our source image is grayscale and the dest one isnt
					memset(dstPx, srcPx[0], channels);
				}
			}
			else {
				float outAlpha = srcAlpha + dstAlpha * (1 - srcAlpha);
				if(outAlpha < .01) {
					memset(dstPx, 0, channels);
				}
				else {
					for(int chnl = 0;chnl < channels;++chnl) {
						dstPx[chnl] = (uint8_t)BYTE_BOUND((srcPx[chnl]/255.f * srcAlpha + dstPx[chnl]/255.f * dstAlpha * (1 - srcAlpha)) / outAlpha * 255.f);
					}
					if(channels > 3) {dstPx[3] = (uint8_t)BYTE_BOUND(outAlpha * 255.f);}
				}
			}
			
		}

	}

	return *this;
}




Image& Image::overlayText(const char* txt, const Font& font, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	size_t len = strlen(txt);
	SFT_Char c;
	int32_t dx, dy;
	uint8_t* dstPx;
	uint8_t srcPx;
	uint8_t color[4] = {r, g, b, a};

	for(size_t i = 0;i < len;++i) {
		if(sft_char(&font.sft, txt[i], &c) != 0) {
			printf("\e[31m[ERROR] Font is missing character '%c'\e[0m\n", txt[i]);
			continue;
		}

		for(uint16_t sy = 0;sy < c.height;++sy) {
			dy = sy + y + c.y;
			if(dy < 0) {continue;}
			else if(dy >= h) {break;}
			for(uint16_t sx = 0;sx < c.width;++sx) {
				dx = sx + x + c.x;
				if(dx < 0) {continue;}
				else if(dx >= w) {break;}
				dstPx = &data[(dx + dy * w) * channels];
				srcPx = c.image[sx + sy * c.width];

				if(srcPx != 0) {
					float srcAlpha = (srcPx / 255.f) * (a / 255.f);
					float dstAlpha = channels < 4 ? 1 : dstPx[3] / 255.f;
					if(srcAlpha > .99 && dstAlpha > .99) {
						memcpy(dstPx, color, channels);
					}
					else {
						float outAlpha = srcAlpha + dstAlpha * (1 - srcAlpha);
						if(outAlpha < .01) {
							memset(dstPx, 0, channels);
						}
						else {
							for(int chnl = 0;chnl < channels;++chnl) {
								dstPx[chnl] = (uint8_t)BYTE_BOUND((color[chnl]/255.f * srcAlpha + dstPx[chnl]/255.f * dstAlpha * (1 - srcAlpha)) / outAlpha * 255.f);
							}
							if(channels > 3) {dstPx[3] = (uint8_t)BYTE_BOUND(outAlpha * 255.f);}
						}
					}
				}
			}
		}

		x += c.advance;
		free(c.image);
	}


	return *this;
}







