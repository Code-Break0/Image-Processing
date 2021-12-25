///*
#include "Image.h"


int main(int argc, char** argv) {
	Image test("imgs/test1.jpg");

	/*
	
		Add your own code here to do some cool stuff with images!
		https://www.youtube.com/playlist?list=PLG5M8QIx5lkzdGkdYQeeCK__As6sI2tOY
		
	*/




	return 0;
}
//*/

//Code for edge detector:
/*
#include "Image.h"

#include <cstdlib>
#include <cmath>
#include <chrono>


int main(int argc, char** argv) {

	Image img("imgs/test6.png");

	
	// grayscale
	img.grayscale_avg();
	int img_size = img.w*img.h;

	Image gray_img(img.w, img.h, 1);
	for(uint64_t k=0; k<img_size; ++k) {
		gray_img.data[k] = img.data[img.channels*k];
	}
	gray_img.write("imgs/test6_gray.png");

	
	// blur
	Image blur_img(img.w, img.h, 1);
	double gauss[9] = {
		1/16., 2/16., 1/16.,
		2/16., 4/16., 2/16.,
		1/16., 2/16., 1/16.
	};
	gray_img.convolve_linear(0, 3, 3, gauss, 1, 1);
	for(uint64_t k=0; k<img_size; ++k) {
		blur_img.data[k] = gray_img.data[k];
	}
	blur_img.write("imgs/test6_blur.png");


	// edge detection
	double* tx = new double[img_size];
	double* ty = new double[img_size];
	double* gx = new double[img_size];
	double* gy = new double[img_size];

	//seperable convolution
	for(uint32_t c=1; c<blur_img.w-1; ++c) {
		for(uint32_t r=0; r<blur_img.h; ++r) {
			tx[r*blur_img.w+c] = blur_img.data[r*blur_img.w+c+1] - blur_img.data[r*blur_img.w+c-1];
			ty[r*blur_img.w+c] = 47*blur_img.data[r*blur_img.w+c+1] + 162*blur_img.data[r*blur_img.w+c] + 47*blur_img.data[r*blur_img.w+c-1];
		}
	}
	for(uint32_t c=1; c<blur_img.w-1; ++c) {
		for(uint32_t r=1; r<blur_img.h-1; ++r) {
			gx[r*blur_img.w+c] = 47*tx[(r+1)*blur_img.w+c] + 162*tx[r*blur_img.w+c] + 47*tx[(r-1)*blur_img.w+c];
			gy[r*blur_img.w+c] = ty[(r+1)*blur_img.w+c] - ty[(r-1)*blur_img.w+c];
		}
	}

	delete[] tx;
	delete[] ty;

	//make test images
	double mxx = -INFINITY,
		mxy = -INFINITY,
		mnx = INFINITY,
		mny = INFINITY;
	for(uint64_t k=0; k<img_size; ++k) {
		mxx = fmax(mxx, gx[k]);
		mxy = fmax(mxy, gy[k]);
		mnx = fmin(mnx, gx[k]);
		mny = fmin(mny, gy[k]);
	}
	Image Gx(img.w, img.h, 1);
	Image Gy(img.w, img.h, 1);
	for(uint64_t k=0; k<img_size; ++k) {
		Gx.data[k] = (uint8_t)(255*(gx[k]-mnx)/(mxx-mnx));
		Gy.data[k] = (uint8_t)(255*(gy[k]-mny)/(mxy-mny));
	}
	Gx.write("imgs/Gx.png");
	Gy.write("imgs/Gy.png");


	// fun part
	double threshold = 0.09;
	double* g = new double[img_size];
	double* theta = new double[img_size];
	double x, y;
	for(uint64_t k=0; k<img_size; ++k) {
		x = gx[k];
		y = gy[k];
		g[k] = sqrt(x*x + y*y);
		theta[k] = atan2(y, x);
	}

	//make images
	double mx = -INFINITY,
		mn = INFINITY;
	for(uint64_t k=0; k<img_size; ++k) {
		mx = fmax(mx, g[k]);
		mn = fmin(mn, g[k]);
	}
	Image G(img.w, img.h, 1);
	Image GT(img.w, img.h, 3);

	double h, s, l;
	double v;
	for(uint64_t k=0; k<img_size; ++k) {
		//theta to determine hue
		h = theta[k]*180./M_PI + 180.;

		//v is the relative edge strength
		if(mx == mn) {
			v = 0;
		}
		else {
			v = (g[k]-mn)/(mx-mn) > threshold ? (g[k]-mn)/(mx-mn) : 0;
		}
		s = l = v;

		//hsl => rgb
		double c = (1-abs(2*l-1))*s;
		double x = c*(1-abs(fmod((h/60),2)-1));
		double m = l-c/2;

		double rt, gt, bt;
		rt=bt=gt = 0;
		if(h < 60) {
			rt = c;
			gt = x;
		}
		else if(h < 120) {
			rt = x;
			gt = c;
		}
		else if(h < 180) {
			gt = c;
			bt = x;
		}
		else if(h < 240) {
			gt = x;
			bt = c;
		}
		else if(h < 300) {
			bt = c;
			rt = x;
		}
		else {
			bt = x;
			rt = c;
		}

		uint8_t red, green, blue;
		red = (uint8_t)(255*(rt+m));
		green = (uint8_t)(255*(gt+m));
		blue = (uint8_t)(255*(bt+m));

		GT.data[k*3] = red;
		GT.data[k*3+1] = green;
		GT.data[k*3+2] = blue;

		G.data[k] = (uint8_t)(255*v);
	}
	G.write("imgs/G.png");
	GT.write("imgs/GT.png");

	delete[] gx;
	delete[] gy;
	delete[] g;
	delete[] theta;

	
	return 0;
}
//*/





