#include "Image.h"


int main(int argc, char** argv) {
	Image test("imgs/test1.jpg");

	



















	// double ker[] = { -2/9.0, -1/9.0, 0,
	// 				 -1/9.0,  1/9.0, 1/9.0,
	// 				 0     ,  1/9.0, 2/9.0 }; //emboss
	// // double ker[] = { 1/16.0, 2/16.0, 1/16.0,
	// // 				 2/16.0, 4/16.0, 2/16.0,
	// // 				 1/16.0, 2/16.0, 1/16.0 }; //gaussian blur

	// Image ct0 = test;
	// ct0.std_convolve_clamp_to_0(0, 3, 3, ker);
	// ct0.std_convolve_clamp_to_0(1, 3, 3, ker);
	// ct0.std_convolve_clamp_to_0(2, 3, 3, ker);
	// ct0.write("imgs/ct0.png");

	// Image ctb = test;
	// ctb.std_convolve_clamp_to_border(0, 3, 3, ker);
	// ctb.std_convolve_clamp_to_border(1, 3, 3, ker);
	// ctb.std_convolve_clamp_to_border(2, 3, 3, ker);
	// ctb.write("imgs/ctb.png");

	// Image ctc = test;
	// ctc.std_convolve_cyclic(0, 3, 3, ker);
	// ctc.std_convolve_cyclic(1, 3, 3, ker);
	// ctc.std_convolve_cyclic(2, 3, 3, ker);
	// ctc.write("imgs/ctc.png");


	// Image diff = ctb;
	// diff.diffmap_scale(ctc);
	// diff.write("imgs/diff.png");

	return 0;
}
