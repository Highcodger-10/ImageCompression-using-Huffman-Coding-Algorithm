#include <iostream>
#include <opencv2/opencv.hpp>
#include "compress.cpp"
using namespace cv;
int main(int argc, char** argv)
{

	Compressor c;
  // char* str = "D:/4thsem/4th semester/DSA_ImageCompressionproject/random.bmp" ;
	// c.compress(str);
  // Mat3b image1 = imread(argv[1], 0);
  // Mat1b image1_gray,image2;
  // cvtColor(image1, image1_gray, COLOR_BGR2GRAY);
    	// show_histogram(argv[1], image1_gray);
  char* compressedfile = "D:/4thsem/4th semester/DSA_ImageCompressionproject/random.compressed";
  c.decompress(compressedfile);
  // waitKey();
	return 0;
}