// KNN_tester.cpp
// from KNN tutorial by Chris Dahms
// https://github.com/MicrocontrollersAndMore
// modified to use larger character set and large font training base
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/ml/ml.hpp>
#include "ContourWithData.h"

#include<iostream>
#include<sstream>




int main() {
	std::vector<ContourWithData> allContoursWithData;			
	std::vector<ContourWithData> validContoursWithData;			

																
	cv::Mat matClassificationFloats;	

	cv::FileStorage fsClassifications("classifications.xml", cv::FileStorage::READ);		

	if (fsClassifications.isOpened() == false) {													
		std::cout << "error, unable to open training classifications file, exiting program\n\n";	
		return(0);																					
	}

	fsClassifications["classifications"] >> matClassificationFloats;		
	fsClassifications.release();											

																			

	cv::Mat matTrainingImages;			

	cv::FileStorage fsTrainingImages("images.xml", cv::FileStorage::READ);			

	if (fsTrainingImages.isOpened() == false) {													
		std::cout << "error, unable to open training images file, exiting program\n\n";			
		return(0);																				
	}

	fsTrainingImages["images"] >> matTrainingImages;				
	fsTrainingImages.release();										

																

	cv::Ptr<cv::ml::KNearest> kNearest = cv::ml::KNearest::create();					
	cv::Ptr<cv::ml::TrainData> trainingData = cv::ml::TrainData::create(matTrainingImages, cv::ml::SampleTypes::ROW_SAMPLE, matClassificationFloats);
	kNearest->setIsClassifier(true);
	kNearest->setAlgorithmType(cv::ml::KNearest::Types::BRUTE_FORCE);
	kNearest->setDefaultK(101);
	cv::Mat matResults(0, 0, CV_32F);
															
	kNearest->train(trainingData);		
																	

																	

	cv::Mat matTestingNumbers = cv::imread("test_numbers.png");		

	if (matTestingNumbers.empty()) {								
		std::cout << "error: image not read from file\n\n";			
		return(0);													
	}

	cv::Mat matGrayscale;			
	cv::Mat matBlurred;				
	cv::Mat matThresh;				
	cv::Mat matThreshCopy;			

	cv::cvtColor(matTestingNumbers, matGrayscale, CV_BGR2GRAY);		

																	
	threshold(matGrayscale, matThresh, 10, 255, CV_THRESH_BINARY_INV);								

	matThreshCopy = matThresh.clone();					

	std::vector<std::vector<cv::Point> > ptContours;		
	std::vector<cv::Vec4i> v4iHierarchy;					

	cv::findContours(matThreshCopy,					
		ptContours,					
		v4iHierarchy,					
		CV_RETR_EXTERNAL,				
		CV_CHAIN_APPROX_TC89_KCOS);		

	for (int i = 0; i < ptContours.size(); i++) {			
		ContourWithData contourWithData;												
		contourWithData.ptContour = ptContours[i];										
		contourWithData.boundingRect = cv::boundingRect(contourWithData.ptContour);		
		contourWithData.fltArea = cv::contourArea(contourWithData.ptContour);			
		allContoursWithData.push_back(contourWithData);									
	}

	for (int i = 0; i < allContoursWithData.size(); i++) {					
		if (allContoursWithData[i].checkIfContourIsValid()) {				
			validContoursWithData.push_back(allContoursWithData[i]);		
		}
	}
	
	std::sort(validContoursWithData.begin(), validContoursWithData.end(), ContourWithData::sortByBoundingRectXPosition);

	std::string strFinalString;			

	for (int i = 0; i < validContoursWithData.size(); i++) {			

																		
		cv::rectangle(matTestingNumbers,				
			validContoursWithData[i].boundingRect,		
			cv::Scalar(0, 255, 0),						
			2);											

		cv::Mat matROI = matThresh(validContoursWithData[i].boundingRect);		

		cv::Mat matROIResized;
		cv::resize(matROI, matROIResized, cv::Size(RESIZED_IMAGE_WIDTH, RESIZED_IMAGE_HEIGHT));		

		cv::Mat matROIFloat;
		matROIResized.convertTo(matROIFloat, CV_32FC1);				

																	
		float fltCurrentChar = kNearest->findNearest(matROIFloat.reshape(1,1), kNearest->getDefaultK(), matResults);							

		strFinalString = strFinalString + char(int(fltCurrentChar));		
	}

	cv::Mat string_box(100,500,CV_8UC3, cv::Scalar::all(0));
	int baseLine = 0;

	cv::Size string_size = cv::getTextSize(strFinalString, CV_FONT_HERSHEY_DUPLEX, 1, 2, &baseLine);
	baseLine += 2;
	cv::Point textOrg((string_box.cols - string_size.width) / 2, (string_box.rows + string_size.height) / 2);

	cv::putText(string_box, strFinalString, textOrg, CV_FONT_HERSHEY_DUPLEX, 1, cv::Scalar::all(255), 2, 8);
	cv::imshow("result", string_box);

	cv::imshow("matTestingNumbers", matTestingNumbers);		

	cv::waitKey(0);											

	return(0);
}