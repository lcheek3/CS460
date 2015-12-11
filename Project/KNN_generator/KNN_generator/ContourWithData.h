#ifndef CONTOURWITHDATA_H_INCLUDED
#define CONTOURWITHDATA_H_INCLUDED

const int MIN_CONTOUR_AREA = 100;

const int RESIZED_IMAGE_WIDTH = 20;
const int RESIZED_IMAGE_HEIGHT = 30;
///////////////////////////////////////////////////////////////////////////////////////////////////
class ContourWithData {
public:
	// member variables ///////////////////////////////////////////////////////////////////////////
	std::vector<cv::Point> ptContour;			// contour
	cv::Rect boundingRect;						// bounding rect for contour
	float fltArea;								// area of contour

												///////////////////////////////////////////////////////////////////////////////////////////////
	bool ContourWithData::checkIfContourIsValid() {									// obviously in a production grade program
		if (fltArea < MIN_CONTOUR_AREA) return false;				// we would have a much more robust function for 
		return true;												// identifying if a contour is valid !!
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	static bool sortByBoundingRectXPosition(const ContourWithData& cwdLeft, const ContourWithData& cwdRight) {		// this function allows us to sort
		return(cwdLeft.boundingRect.x < cwdRight.boundingRect.x);													// the contours from left to right
	}
};
#endif
