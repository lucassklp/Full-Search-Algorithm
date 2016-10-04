#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <list>
#include <limits>
#include <cmath>

using namespace std;
using namespace cv;

void OpenImage(Mat image, string windowName) {
	namedWindow(windowName, WINDOW_AUTOSIZE);
	imshow(windowName, image);
}	

int Evaluate(Rect windowSearch, Rect block, Mat Image1, Mat Image2) {
	
	int evaluationFactor = 0;

	for (int i = windowSearch.x; i < windowSearch.x + windowSearch.width; i++)
	{
		for (int j = windowSearch.y; j < windowSearch.y + windowSearch.height; j++)
		{
			int difference = std::pow(Image2.at<uchar>(i, j) - Image1.at<uchar>(i, j), 2);
			evaluationFactor += difference;
		}
	}
	//std::cout << "Evaluation: " << evaluationFactor << endl;

	return evaluationFactor;
}


void FullSearch(Mat image, Mat image2, int blockSize, int searchWindowSize) {
	list<Rect> blocks = list<Rect>();
	
	for (int line = 0; line < image.rows; line+=blockSize)
	{
		for (int column = 0; column < image.cols; column+=blockSize)
		{
			Rect p = Rect(line, column, blockSize, blockSize);
			blocks.push_back(p);
		}
	}

	int minorValue = std::numeric_limits<int>::max();
	for (list<Rect>::iterator block = blocks.begin(); block != blocks.end(); block++)
	{
		Point mean = Point(block->x + blockSize / 2, block->y + blockSize / 2);
		
		Point searchWindowAxis = Point(mean.x - searchWindowSize / 2, mean.y - searchWindowSize / 2);
		
		Rect searchWindow = Rect(searchWindowAxis.x, searchWindowAxis.y, searchWindowSize, searchWindowSize);

		for (int i = searchWindow.x; i < searchWindow.x + searchWindowSize; i++)
		{
			if (searchWindow.x >= 0) 
			{
				for (int j = searchWindow.y; j < searchWindow.y + searchWindowSize; j++)
				{
					if (searchWindow.y >= 0) 
					{
						int evaluateValue = Evaluate(Rect(i, j, searchWindowSize, searchWindowSize), Rect(block->x, block->y, block->width, block->height), image, image2);
						
						if (evaluateValue < minorValue) {
							minorValue = evaluateValue;
						}
					}
				}
			}
		}
		std::cout << "O menor valor da avaliacao do bloco foi  " << minorValue << endl;
	}
	
	
	//for (int i = 0; i < image.rows; i++)
	//{
	//	for (int j = 0; j < image.cols; j++)
	//	{
	//		int grayScale = image.at<uchar>(i, j);
	//		if (grayScale < 100) {
	//		}
	//	}
	//}
	//OpenImage(image, "White");
}

int main(int argc, char** argv) {
	Mat image = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
	Mat image2 = imread(argv[2], CV_LOAD_IMAGE_GRAYSCALE);

	FullSearch(image, image2, 32, 128);

	OpenImage(image, "Imagem 1");
	OpenImage(image2, "Imagem 2");



	waitKey(0);
}


