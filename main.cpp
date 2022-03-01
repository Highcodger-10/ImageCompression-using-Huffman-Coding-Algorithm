#include <iostream>
#include <opencv2/opencv.hpp>
#include "compress.cpp"
int main(int argc, char** argv)
{

	Compressor c;
  // char* path = "";
  // int selector;
  // cout << "Enter 0 for compression and 1 for decompression" << endl;
  // cin >> selector;
  // if(selector == 0){
  //   cout << "You have opted for the compression of images.Please enter the path for the image to be compressed. " << endl;
  //   cin >> path;
  //   c.compress(path);
  // }
  // else if(selector == 1){
  //   cout << "You have opted for the decompression of images. Please enter the path for the image to be extracted." << endl;
  //   cin >> path;
  //   c.decompress(path);
  // }
  // else{
  //   cout << "Select only 0 or 1 as asked "<< endl;
  // }
  // char* str = "D:/4thsem/4th semester/DSA_ImageCompressionproject/random.bmp" ;
	// c.compress(str);
  // Mat3b image1 = imread(argv[1], 0);
  // Mat1b image1_gray,image2;
  // cvtColor(image1, image1_gray, COLOR_BGR2GRAY);
    	// show_histogram(argv[1], image1_gray);
  char* compressedfile = "D:/4thsem/4th semester/DSA_ImageCompressionproject/random.compressed";
  c.decompress(compressedfile);
  // // waitKey();
	return 0;
}