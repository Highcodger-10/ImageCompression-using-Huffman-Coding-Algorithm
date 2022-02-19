// #include <opencv2/opencv.hpp>
// #include <iostream>

// using namespace cv;
// using namespace std;

// int main(int argc, char **argv)
// {
// 	// Read the image file
// 	Mat image = imread("D:/4thsem/4th semester/opencvtut/ironman.png");
// 	Mat debug_image(5,5,CV_8UC3);
// 	Mat gray_scale;
// 	Mat binary_image;
// 	// namedWindow("showbinary");
// 	// namedWindow("Original");
// 	cvtColor(image, gray_scale, COLOR_BGR2GRAY);
// 	threshold(gray_scale, binary_image, 100, 255, THRESH_BINARY);
// 	// Check for failure
// 	if (image.empty())
// 	{
// 		cout << "Image Not Found!!!" << endl;
// 		cin.get(); // wait for any key press
// 		return -1;
// 	}
// 	// Show our image inside a window.
// 	// imshow("Original", image);
// 	// imshow("Binary", binary_image);
// 	// imshow("Grayscale", gray_scale);
// 	// cout << image.rows << endl;//rows and cols represent the rows and cloumns of the image
// 	// cout << image.channels() << endl;//channels() is a method of the Mat class
// 	// cout << image.cols << endl;
// 	// cout << image << endl;
// 	// Wait for any keystroke in the window
// 	cout << debug_image << endl;
// 	waitKey(0);
// 	return 0;
// }

#include<iostream>
#include<opencv2/opencv.hpp>
#include"compress.cpp"
#include<cstring>
using namespace std;
int main(int argc, char** argv){
	Compressor c;
  char* lena = "D:/4thsem/4th semester/DSA_ImageCompressionproject/Prashant.jpg";
	c.compress(lena);
  // c.compress(argv[1]);
  Mat3b image1 = imread(lena, 0);
  Mat1b image1_gray,image2;
  cvtColor(image1, image1_gray, COLOR_BGR2GRAY);
  // show_histogram(argv[1], image1_gray);
  waitKey();
	return 0;
}