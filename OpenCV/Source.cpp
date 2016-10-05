#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <list>
#include <limits>
#include <cmath>
#include <thread>

using namespace std;
using namespace cv;

void OpenImage(Mat image, string windowName) {
	namedWindow(windowName, WINDOW_AUTOSIZE);
	imshow(windowName, image);
}	

int EvaluateSAD(Rect currentBlock, Rect referencialBlock, Mat Image1, Mat Image2) {
	
	int SAD = 0;

	for (int i = currentBlock.x; i < currentBlock.x + currentBlock.width; i++)
	{
		for (int j = currentBlock.y; j < currentBlock.y + currentBlock.height; j++)
		{
			int incrementX = i - currentBlock.x;
			int incrementY = j - currentBlock.y;
			int img1 = Image1.at<uchar>(referencialBlock.x + incrementX , referencialBlock.y + incrementY);
			int img2 = Image2.at<uchar>(i, j);
			
			int difference = std::pow((img2 - img1), 2);
			SAD += difference;
		}
	}

	return SAD;
}

Rect WindowSearch(Rect block, Rect blockSize, Rect searchWindowSize, Rect searchWindow, Mat image, Mat image2) {

	int minorValue = std::numeric_limits<int>::max();
	Rect matchingBlock;

	for (int i = searchWindow.x; i < searchWindow.x + searchWindowSize.width; i++)
	{
		if (i >= 0 && i < image.rows)
		{
			for (int j = searchWindow.y; j < searchWindow.y + searchWindowSize.height; j++)
			{
				if (j >= 0 && j < image.cols)
				{
					int SADValue = EvaluateSAD(Rect(i, j, blockSize.width, blockSize.height), block, image, image2);

					if (SADValue < minorValue) {
						minorValue = SADValue;
						matchingBlock = Rect(i, j, blockSize.width, blockSize.height);
					}
				}
			}
		}
	}


	std::cout << "Bloco analizado: x = " << block.x << ", y = " << block.y << endl;
	std::cout << "Menor valor: " << minorValue << endl;
	std::cout << "Best Matching: x = " << matchingBlock.x << ", y = " << matchingBlock.y << endl << endl;

	return matchingBlock;
}



void FullSearch(Mat image, Mat image2, Rect blockSize, Rect searchWindowSize) {
	list<Rect> blocks = list<Rect>();
	
	for (int line = 0; line < image.rows; line+=blockSize.width)
	{
		for (int column = 0; column < image.cols; column+=blockSize.height)
		{
			Rect p = Rect(line, column, blockSize.width, blockSize.height);
			blocks.push_back(p);
		}
	}

	list<Rect> matchingBlocks;
	Mat resultImage = Mat(image.rows, image.cols, image.type());
	for (list<Rect>::iterator block = blocks.begin(); block != blocks.end(); block++)
	{
		Point mean = Point(block->x + blockSize.width / 2, block->y + blockSize.height / 2);
		Point searchWindowAxis = Point(mean.x - searchWindowSize.width / 2, mean.y - searchWindowSize.height / 2);		
		Rect searchWindow = Rect(searchWindowAxis.x, searchWindowAxis.y, searchWindowSize.width, searchWindowSize.height);
		
		thread AnalyzeWindow(WindowSearch, Rect(block->x, block->y, block->width, block->height), blockSize, searchWindowSize, searchWindow, image, image2);


		//Mat ROIAnalyzedBlock = image(p);
		//Mat ROIBestMatchingBlock = image(matchingBlock);
		//Mat ROIOriginalImage = image2(p);

		//OpenImage(ROIAnalyzedBlock, "Bloco analizado");
		//OpenImage(ROIOriginalImage, "Imagem Original");
		//OpenImage(ROIBestMatchingBlock, "Best-matching");
		//waitKey(0);

		//ROIBestMatchingBlock.copyTo(resultImage);
		




		//std::cin.get();




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
	
	FullSearch(image, image2, Rect(0, 0, 32, 32), Rect(0, 0, 128, 128));

	OpenImage(image, "Imagem 1");
	OpenImage(image2, "Imagem 2");



	waitKey(0);
}


