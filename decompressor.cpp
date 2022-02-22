#include<iostream>
#include<opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <cstring>
#include <fstream>
#include <queue>
#include <map>
#include <vector>
#include <exception>
#include "huffman.hpp"
using namespace std;
using namespace cv;
class Decompressor{
	Huffmantree h;
	Mat gray_image; // monochromatic version of the original image
	Mat dct_image;  // DCT of the monochromatic image
	int height, DCT_height, width, DCT_width;
	int C;		  // compression parameter
	int *zigzagmasking; // masking for zigzag search in an image
	string *shiftedbit; // masking for shifted bits in a string
	unsigned int numberofbitsshifted;
	Mat luminance;				  // Quantization matrix for luminance which has a fix value as defined in the constructor
	priority_queue<node> frequency_table; // frequency table for huffman coding
	ofstream outfile;				  // to write in .compressed binary file used to compress
	ifstream infile;				  // to read from .compressed binary file used to decompress
	string coded_image;			  // image after huffman codification
	string filename;				  // name of the image file .compressed
	vector<float> codetable;		  // for frequency of each node

	public:
	Decompressor(){
		// to add zeros to the end of the file to actually complete a byte
		// static string bitMask[10] = {"", "0", "00", "000", "0000", "00000", "000000", "0000000", "00000000"};
		// shiftedbit = bitMask;
		// to place the DCT image in zigzag order, we use zzmask
		static int zzMask[64] = {
		    0, 1, 8, 16, 9, 2, 3, 10,
		    17, 24, 32, 25, 18, 11, 4, 5,
		    12, 19, 26, 33, 40, 48, 41, 34,
		    27, 20, 13, 6, 7, 14, 21, 28,
		    35, 42, 49, 56, 57, 50, 43, 36,
		    29, 22, 15, 23, 30, 37, 44, 51,
		    58, 59, 52, 45, 38, 31, 39, 46,
		    53, 60, 61, 54, 47, 55, 62, 63};
		zigzagmasking = zzMask;
		// luminance is to perform the quantization and rounding off the transformed sub-image(matrix)
		luminance = (Mat_<float>(8, 8) << 16, 11, 10, 16, 24, 40, 51, 61,
				12, 12, 14, 19, 26, 58, 60, 55,
				14, 13, 16, 24, 40, 57, 69, 56,
				14, 17, 22, 29, 51, 87, 80, 62,
				18, 22, 37, 56, 68, 109, 103, 77,
				24, 35, 55, 64, 81, 104, 113, 92,
				49, 64, 78, 87, 103, 121, 120, 101,
				72, 92, 95, 98, 112, 100, 103, 99);
		// width = 0;
		// height = 0;
		// DCT_height = 0;
		// DCT_width = 0;
		C = 1; // Compress rate - [1 Maximal - 8 Minimal]
		// numberofbitsshifted = 0;
	}

	
		void setparameter_decompression(char* imagename){
			filename.clear();
			filename.append(imagename);
			if(filename.substr(filename.length()-11,filename.length())!=".compressed"){
				cout << "\nCan't load the image, Please insert the proper address "<< endl;
				throw exception();
			}
			filename.erase(filename.length()-11,filename.length());
			filename += ".png";
			infile.open(imagename);
			if (!infile.is_open()){
				cout << "\nCan't load the image. Please insert the proper address." << endl;
				throw exception();
			}
			string size;

			//converting string to an integer to get height width, dct height and dct width
			infile>>size;
			height = atoi(size.c_str());
			infile >> size;
			width = atoi(size.c_str());
			infile >> size;
			DCT_height = atoi(size.c_str());
			infile >> size;
			DCT_width = atoi(size.c_str());
		}

		void readcodetable(){
			string ind, frequency;
			infile >> ind;
			while(ind[0]!= '#'){
				infile >> frequency;
				this->h.setfrequencytable(atoi(ind.c_str()), strtof(frequency.c_str(),0));
				infile >> ind;
			}
			ind.erase(0,1);	//to remove #
			numberofbitsshifted = atol(ind.c_str());
		}

		void readcodedimage(){
			unsigned char c;
			unsigned char mask = 128;
			int count = 0;
			infile >> noskipws;	//takes in white space so that the file pointer can move forward to read coded image
			infile >> c;
			while(infile >> c){
				for (int i = 0; i < 8; i++){
					if ((c<<i) && mask)
						coded_image.push_back('1');
					else	
						coded_image.push_back('0');
					count++;
				}
			}
			// cout << coded_image.length();
			coded_image.erase(coded_image.length()- numberofbitsshifted, numberofbitsshifted);	//to remove the extra bits which were masked before to complete a byte in compression process
		}
		
		void inversedct(){
			gray_image = Mat_<uchar>(height,width);
			for (int i = 0; i < height*C/8; i+= C){
				for (int j = 0; j< width*C/8; j+=C){
					//getting an 8*8 block for each position on image for inverse transformation
					Mat block = dct_image(Rect(j,i,C,C));
					//converting from 8 bits to 64 bits for actual inverse dct
					block.convertTo(block,CV_32FC1);
					//subtracting the block by 128
					subtract(block,128,block);
					Mat block8 = Mat::zeros(8,8,CV_32FC1);
					for (int k = 0;k < (int)C*C; k++){
						block8.at<float>(zigzagmasking[k]) = block.at<float>(k);				
					}

					//Quantization step
					multiply(block8,luminance,block8);

					idct(block8,block8);

					//converting back to unsigned char
					block8.convertTo(block8,CV_8UC1);

					//copying the block back to the gray image
					block8.copyTo(gray_image(Rect(8*j/C,8*i/C,8,8)));
				}
			}
		}

		//actual decompression
		void decompress(char* filename){
			cout << "\nExtracting..." << endl;
			cout << "Setting Parameters for extraction..."<< endl;
			setparameter_decompression(filename);
			cout << "Reading code table..." << endl;
			readcodetable();
			cout << "Reading the binary image..." << endl;
			readcodedimage();
			cout << "Decoding the binary file..." << endl;

			
			vector<int> dctfile = this->h.decode(coded_image);
			dct_image = Mat::zeros(height*C/8,width*C/8,CV_8UC1);
			cout << "Yo print vayacha" << endl;
			//yaa baata faseyako cha. Yo baata continue garna parcha
			for (int i = 0; i < height*C/8; i++){
				for (int j = 0; j < width*C/8; j++){
					//yetai kehi gadbadi cha jasto dekhencha kina vandaa yetai kehi steps pachi adkeraacha program
					dct_image.at<uchar>(i,j) = (uchar)dctfile.at(i*dct_image.step + j);//at function le pakkai pani out of range exception throw gareko huna parcha katai tera
				}
			} 

			cout << "Hello" << endl;
			//actual processing of binary file
			inversedct();
			if(DCT_width != 0){
				gray_image = gray_image.colRange(0,width- DCT_width);
			}
			if(DCT_height!= 0){
				gray_image = gray_image.rowRange(0,height-DCT_height);
			}
			imwrite(this->filename,gray_image);
			Mat testcolor = imread(this->filename);
			cout << "Image build successfully" << endl;
			// displayImage(this->filename,testcolor);
		}

		// Display all images
		void displayImage(string imageName, Mat image){
			/// Display
			namedWindow(imageName,WINDOW_AUTOSIZE);
			imshow(imageName, image);
			waitKey(0);
		}
};