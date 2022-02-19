#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <cstring>
#include <fstream>
#include <queue>
#include <map>
#include <vector>
#include <exception>
#include "huffman.cpp"
using namespace std;
using namespace cv;
class Compressor
{
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
	Compressor()
	{
		// to add zeros to the end of the file to actually complete a byte
		static string bitMask[10] = {"", "0", "00", "000", "0000", "00000", "000000", "0000000", "00000000"};
		shiftedbit = bitMask;
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
		width = 0;
		height = 0;
		DCT_height = 0;
		DCT_width = 0;
		C = 1; // Compress rate - [1 Maximal - 8 Minimal]
		numberofbitsshifted = 0;
	} // to set masking and the quantization and intialize other variables
	virtual ~Compressor(){};
	void setprarameter_compression(char *imagename)
	{
		gray_image = imread(imagename, IMREAD_GRAYSCALE);

		// checking if the image actually exists
		if (!gray_image.data)
		{
			cout << "Cannot open the image as it doesn't exist. Please insert the proper image address" << endl;
			throw exception();
		}

		// modifying the image to a file with .compressed extension
		filename.clear();
		filename.append(imagename);
		filename.erase(filename.end() - 4, filename.end()); // given the starting range and ending range for deletion
		filename += ".compressed";

		// open output stream to .compressed
		outfile.open(filename.c_str()); // here, c_str returns a pointer to the string

		// to break the block of image into 8*8 to perform DCT we transform the resoluton of the image to an image whose resolution is a multiple of 8*8
		height = gray_image.size().height;
		width = gray_image.size().width;
		DCT_height = (height % 8) ? (DCT_height = 8 - height % 8) : 0;
		DCT_width = (width % 8) ? (DCT_width = 8 - width % 8) : 0;
		if (DCT_height != 0)
		{
			Mat hblock = gray_image(Rect(0, height - DCT_height, width, DCT_height));
			vconcat(gray_image, hblock, gray_image);
			height += DCT_height;
		}
		if (DCT_width != 0)
		{
			Mat wblock = gray_image(Rect(width - DCT_width, 0, DCT_width, height));
			hconcat(gray_image, wblock, gray_image);
			width += DCT_width;
		}

		// writing to our .compressed file
		outfile << height << " " << width << endl;
		outfile << DCT_height << " " << DCT_width << endl;
	}
	// void setparameter_decompression(char* imagename);

	// forward DCT transformation
	void forwardDCT()
	{
		dct_image = Mat_<uchar>(height * C / 8, width * C / 8);
		for (int i = 0; i < height; i += 8)
		{
			for (int j = 0; j < width; j += 8)
			{
				// getting a 8*8 block for each position on the image
				Mat block8 = gray_image(Rect(j, i, 8, 8));
				// converting block from 8 bits to 64 bits
				block8.convertTo(block8, CV_32FC1);
				// performing DCT
				dct(block8, block8);
				// performing Quantization
				divide(block8, luminance, block8);
				// adding 128.0 to all the elements of the block
				add(block8, 128, block8);
				// converting back to unsigned char
				block8.convertTo(block8, CV_8UC1);
				// copying the block back to the dct_image
				Mat blockC = Mat_<uchar>(C, C);
				// to place the elements in zigzag order
				for (int k = 0; k < (int)C * C; k++)
				{
					blockC.at<uchar>(k) = block8.at<uchar>(zigzagmasking[k]);
				}
				blockC.copyTo(dct_image(Rect(j * C / 8, i * C / 8, C, C)));
			}
		}
		dct_image.convertTo(dct_image, CV_8UC1);
	}
	// void inverseDCT();
	// void displayimage(string imagename, Mat Image);
	void compress(char *imagename)
	{
		cout << "\n Compressing...." << endl;
		cout << "Setting parameter..." << endl;
		setprarameter_compression(imagename);
		cout << "\n Transforming image..." << endl;
		forwardDCT();
		searchfrequencytable();
		codetable = h.gethuffmancode();
		for (unsigned short i = 0; i < codetable.size(); i++)
		{
			if (codetable.at(i) == 0)
			{
				continue;
			}
			outfile << i << " " << codetable.at(i) << endl;
		}

		// transforming image matrix to vector
		vector<int> Inputfile;
		Inputfile.assign(dct_image.datastart, dct_image.dataend);

		// codifying image DCT based on the table of code from huffman tree
		coded_image = h.encode(Inputfile);

		// adding 0s to the end of file to actually complete a byte
		numberofbitsshifted = 8 - coded_image.length() % 8;
		coded_image += shiftedbit[numberofbitsshifted];

		// end of the code table and the number of bits 0 shifted in the end of the file
		outfile << "#" << numberofbitsshifted << endl;

		size_t imsize = coded_image.size();
		for (size_t i = 0; i < imsize; i += 8)
		{
			outfile << (uchar)strtol(coded_image.substr(i, 8).c_str(), 0, 2);
		}
		coded_image.clear();
		outfile.close();

		// Just for information
		if (!coded_image.length())
			cout << "\nCompressing Successful!";
		else
			cout << "\nCompressing Failed!";
	}
	// void decompress(char *imagename);

	// computing the histogram matrix for DCT image in plane so that we could get frequency table
	void searchfrequencytable()
	{
		int histsize = 256;
		float range[] = {0, 256};
		const float *histrange = {range};
		bool uniform = true;
		bool accumulate = false;
		Mat histogram;
		calcHist(&dct_image, 1, 0, Mat(), histogram, 1, &histsize, &histrange, uniform, accumulate);
		vector<float> f(histogram.begin<float>(), histogram.end<float>());
		h.setfrequencytable(f);
	}
	// void read_codetable();
	// void read_codedimage();
};

