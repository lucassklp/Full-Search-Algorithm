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

int Evaluate(Rect comparativeBlock, Rect referencialBlock, Mat Image1, Mat Image2) {
	
	int evaluationFactor = 0;

	for (int i = comparativeBlock.x; i < comparativeBlock.x + comparativeBlock.width; i++)
	{
		for (int j = comparativeBlock.y; j < comparativeBlock.y + comparativeBlock.height; j++)
		{
			int img2 = Image2.at<uchar>(i, j);

			int img1 = Image1.at<uchar>(referencialBlock.x + i - comparativeBlock.x, referencialBlock.y + j - comparativeBlock.y);

			int difference = std::pow((img2 - img1), 2);
			evaluationFactor += difference;
		}
	}

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

	list<Rect> matchingBlocks;

	for (list<Rect>::iterator block = blocks.begin(); block != blocks.end(); block++)
	{
		
		Point mean = Point(block->x + blockSize / 2, block->y + blockSize / 2);
		
		Point searchWindowAxis = Point(mean.x - searchWindowSize / 2, mean.y - searchWindowSize / 2);
		
		Rect searchWindow = Rect(searchWindowAxis.x, searchWindowAxis.y, searchWindowSize, searchWindowSize);
		

		Rect matchingBlock;
		int minorValue = std::numeric_limits<int>::max();
		for (int i = searchWindow.x; i < searchWindow.x + searchWindowSize; i++)
		{
			if (i >= 0) 
			{
				for (int j = searchWindow.y; j < searchWindow.y + searchWindowSize; j++)
				{
					if (j >= 0) 
					{
						int evaluateValue = Evaluate(Rect(i, j, blockSize, blockSize), Rect(block->x, block->y, block->width, block->height), image, image2);
						
						if (evaluateValue < minorValue) {
							minorValue = evaluateValue;
							matchingBlock = Rect(i, j, blockSize, blockSize);
						}

						if (minorValue == 0)
							break;
					}
				}
			}
		}
		matchingBlocks.push_back(matchingBlock);
		std::cout << "Bloco analizado: x = " << block->x << ", y = " << block->y << endl;
		std::cout << "Menor valor: " << minorValue << endl;
		std::cout << "Best Matching: x = " << matchingBlock.x << ", y = " << matchingBlock.y << endl <<endl;

		Rect p = Rect(block->x, block->y, block->width, block->height);
		Mat ROIAnalyzedBlock = image(p);

		Mat ROIBestMatchingBlock = image2(matchingBlock);

		OpenImage(ROIAnalyzedBlock, "Bloco analizado");
		OpenImage(ROIBestMatchingBlock, "Best-matching");
		waitKey(0);

		std::cin.get();




	}


	//Montando as imagens lado a lado





	
	
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

	FullSearch(image, image2, 64, 128);

	OpenImage(image, "Imagem 1");
	OpenImage(image2, "Imagem 2");



	waitKey(0);
}


