// KNN_Generator.cpp
// from KNN tutorial by Chris Dahms
// https://github.com/MicrocontrollersAndMore
// modified to use larger character set and large font training base
//training base source: http://www.ee.surrey.ac.uk/CVSSP/demos/chars74k/
//     T.E.de Campos, B.R.Babu and M.Varma.Character recognition in natural images.
//     In Proceedings of the International Conference on Computer Vision Theory and Applications(VISAPP), Lisbon, Portugal, February 2009.
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/ml/ml.hpp>
#include<sstream>
#include <iomanip>
#include <Windows.h>
#include "ContourWithData.h"

#include<iostream>
#include<vector>

int getKey(char);

int main() {
	std::string order= "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	std::vector<char> characterOrder(order.begin(),order.end());
	cv::Mat matTrainingNumbers;		// input image
	cv::Mat matGrayscale;			// 
	cv::Mat matBlurred;				// declare various images
	cv::Mat matThresh;				//
	cv::Mat matThreshCopy;			//
	std::vector<std::vector<cv::Point> > ptContours;		// declare contours vector
	std::vector<cv::Vec4i> v4iHierarchy;					// declare contours hierarchy
	cv::Mat matClassificationInts;		// these are our training classifications, note we will have to perform some conversions before writing to file later
	int intChar = 0;

										// these are our training images, due to the data types that the KNN object KNearest requires,
	cv::Mat matTrainingImages;					// we have to declare a single Mat, then append to it as though it's a vector,
												// also we will have to perform some conversions before writing to file later
	//dumb file read system (since file and image names are consistent)
	for (int directory_num = 1; directory_num <= 36; directory_num++) {
		intChar = getKey(characterOrder[directory_num - 1]);
		for (int file_num = 1; file_num <= 1016; file_num++) {
			std::stringstream path_stream;
			std::stringstream directory_pad;
			std::stringstream file_pad;
			directory_pad << std::setfill('0') << std::setw(2) << directory_num;
			file_pad << std::setfill('0') << std::setw(4) << file_num;
			std::string file = file_pad.str();
			path_stream << "Img/Sample0" << directory_pad.str() << "/img0" << directory_pad.str() << "-0" << file_pad.str() << ".png";
			std::string path_string = path_stream.str();
			std::cout << path_string << std::endl;
			matTrainingNumbers = cv::imread(path_string);
			if (matTrainingNumbers.empty()) {							// if unable to open image
				std::cout << "error: image not read from file\n\n";		// show error message on command line
				return(0);												// and exit program
			}
			cv::cvtColor(matTrainingNumbers, matGrayscale, CV_BGR2GRAY);		// convert to grayscale

													// filter image from grayscale to black and white
			threshold(matGrayscale, matThresh, 10, 255, CV_THRESH_BINARY_INV);                        
			//cv::imwrite("test.png", matThresh);
			matThreshCopy = matThresh.clone();
			cv::findContours(matThreshCopy,					// input image, make sure to use a copy since the function will modify this image in the course of finding contours
				ptContours,					// output contours
				v4iHierarchy,					// output hierarchy
				CV_RETR_EXTERNAL,				// retrieve the outermost contours only
				CV_CHAIN_APPROX_TC89_KCOS);		// compress horizontal, vertical, and diagonal segments and leave only their end points

			cv::Rect boundingRect = cv::boundingRect(ptContours[0]);			// get the bounding rect

			cv::rectangle(matTrainingNumbers, boundingRect, cv::Scalar(0, 0, 255), 2);		// draw red rectangle around each contour as we ask user for input
			//cv::imwrite("test2.png", matTrainingNumbers);
			cv::Mat matROI = matThresh(boundingRect);			// get ROI image of bounding rect
			cv::Mat matROIResized;
			cv::resize(matROI, matROIResized, cv::Size(RESIZED_IMAGE_WIDTH, RESIZED_IMAGE_HEIGHT));		// resize image, this will be more consistent for recognition and storage
			//cv::imwrite("test3.png", matROIResized);
			matClassificationInts.push_back(intChar);		// append classification char to integer list of chars (we will convert later before writing to file)

			cv::Mat matImageFloat;							// now add the training image (some conversion is necessary first) . . .
			matROIResized.convertTo(matImageFloat, CV_32FC1);		// convert Mat to float

			cv::Mat matImageReshaped = matImageFloat.reshape(1, 1);		// flatten

			matTrainingImages.push_back(matImageReshaped);		// add to Mat as though it was a vector, this is necessary due to the
																// data types that KNearest.train accepts
		}
	}
	std::cout << "training complete" << std::endl;
	cv::Mat matClassificationFloats;
	matClassificationInts.convertTo(matClassificationFloats, CV_32FC1);			// convert ints to floats

	cv::FileStorage fsClassifications("classifications.xml", cv::FileStorage::WRITE);			// open the classifications file

	if (fsClassifications.isOpened() == false) {														// if the file was not opened successfully
		std::cout << "error, unable to open training classifications file, exiting program\n\n";		// show error message
		return(0);																						// and exit program
	}

	fsClassifications << "classifications" << matClassificationFloats;		// write classifications into classifications section of classifications file
	fsClassifications.release();											// close the classifications file

																			// save training images to file ///////////////////////////////////////////////////////

	cv::FileStorage fsTrainingImages("images.xml", cv::FileStorage::WRITE);			// open the training images file

	if (fsTrainingImages.isOpened() == false) {													// if the file was not opened successfully
		std::cout << "error, unable to open training images file, exiting program\n\n";			// show error message
		return(0);																				// and exit program
	}

	fsTrainingImages << "images" << matTrainingImages;		// write training images into images section of images file
	fsTrainingImages.release();								// close the training images file

	return(0);
}

//LOOK NO FARTHER YE WHO ENTER HERE
//lest you see the result of a silly student who didnt read the documentation
//VkScanKey from windows.h would have done this in four lines
//I will leave it as a harrowing reminder

int getKey(char character) {
	switch (character) {
	case '0':
		return 48;
	case '1':
		return 49;
	case '2':
		return 50;
	case '3':
		return 51;
	case '4':
		return 52;
	case '5':
		return 53;
	case '6':
		return 54;
	case '7':
		return 55;
	case '8':
		return 56;
	case '9':
		return 57;
	case 'a':
		return 97;
	case 'b':
		return 98;
	case 'c':
		return 99;
	case 'd':
		return 100;
	case 'e':
		return 101;
	case 'f':
		return 102;
	case 'g':
		return 103;
	case 'h':
		return 104;
	case 'i':
		return 105;
	case 'j':
		return 106;
	case 'k':
		return 107;
	case 'l':
		return 108;
	case 'm':
		return 109;
	case 'n':
		return 110;
	case 'o':
		return 111;
	case 'p':
		return 112;
	case 'q':
		return 113;
	case 'r':
		return 114;
	case 's':
		return 115;
	case 't':
		return 116;
	case 'u':
		return 117;
	case 'v':
		return 118;
	case 'w':
		return 119;
	case 'x':
		return 120;
	case 'y':
		return 121;
	case 'z':
		return 122;
	case 'A':
		return 65;
	case 'B':
		return 66;
	case 'C':
		return 67;
	case 'D':
		return 68;
	case 'E':
		return 69;
	case 'F':
		return 70;
	case 'G':
		return 71;
	case 'H':
		return 72;
	case 'I':
		return 73;
	case 'J':
		return 74;
	case 'K':
		return 75;
	case 'L':
		return 76;
	case 'M':
		return 77;
	case 'N':
		return 78;
	case 'O':
		return 79;
	case 'P':
		return 80;
	case 'Q':
		return 81;
	case 'R':
		return 82;
	case 'S':
		return 83;
	case 'T':
		return 84;
	case 'U':
		return 85;
	case 'V':
		return 86;
	case 'W':
		return 87;
	case 'X':
		return 88;
	case 'Y':
		return 89;
	case 'Z':
		return 90;
	}
	return 0;
}