#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <cmath>
#include <list>
#include <limits>
#include <thread>
#include <stdlib.h>

using namespace std;
using namespace cv;

list<pair<Rect, Rect>> matchingBlocks;
bool isFinished = false;

void OpenImage(Mat image, string windowName) {
	namedWindow(windowName, WINDOW_AUTOSIZE);
	imshow(windowName, image);
}	


void MakeImage(Mat image, Mat image2){
	int width = image.cols;
	int height = image.rows;
	
	
	//Mat resultImage(width, height, image.type());

	for (list<pair<Rect, Rect>>::iterator match = matchingBlocks.begin(); match != matchingBlocks.end(); match++)
	{
		Mat matchBlock = image(match->first);
		Mat roi = image2(match->second);
		matchBlock.copyTo(roi);
	}

	imwrite("result.jpg", image2);


	OpenImage(image2, "Final");
	waitKey(0);

}


void ShowProgress(int count, Mat image, Mat image2) {
	
	while (!isFinished)
	{
		int x = matchingBlocks.size() * 100 / count;
		system("cls");
		std::cout << x << "%";

		if (matchingBlocks.size() == count) {
			isFinished = true;
			MakeImage(image, image2);
		}
	}
}

int EvaluateMSE(Rect currentBlock, Rect referencialBlock, Mat Image1, Mat Image2) {
	
	int SAD = 0;

	for (int y = currentBlock.y; y < currentBlock.y + currentBlock.height; y++)
	{
		for (int x = currentBlock.x; x < currentBlock.x + currentBlock.width; x++)
		{
			int incrementX = x - currentBlock.x;
			int incrementY = y - currentBlock.y;

			Point referencialPoint(referencialBlock.x + incrementX, referencialBlock.y + incrementY);
			Point currentPoint(x, y);

			int img1 = Image1.at<uchar>(referencialPoint);
			int img2 = Image2.at<uchar>(currentPoint);

			int difference = pow(img2 - img1, 2);
			SAD += difference;
		}
	}
	

	return (SAD);
}

void WindowSearch(Rect block, Rect searchWindow, Mat image, Mat image2) {

	int minorValue = std::numeric_limits<int>::max();
	Rect matchingBlock;

	for (int y = searchWindow.y; y < searchWindow.y + searchWindow.height; y++)
	{
		if (y >= 0 && y < image.rows - block.height)
		{
			for (int x = searchWindow.x; x < searchWindow.x + searchWindow.width; x++)
			{
				if (x >= 0 && x < image.cols - block.width)
				{
					int SADValue = EvaluateMSE(Rect(x, y, block.width, block.height), block, image, image2);

					if (SADValue < minorValue) {
						minorValue = SADValue;
						matchingBlock = Rect(x, y, block.width, block.height);
					}
				}
			}
		}
	}

	Mat matchingBlockROI = image(matchingBlock);
	Mat blockROI = image2(block);
	
	//OpenImage(matchingBlockROI, "Matching Block");
	//OpenImage(blockROI, "Original Block");
	//waitKey(0);

	pair<Rect, Rect> blockPair = make_pair(matchingBlock, block);
	matchingBlocks.push_back(blockPair);
}



void FullSearch(Mat image, Mat image2, Rect blockSize, Rect searchWindowSize) {
	list<Rect> blocks = list<Rect>();
	
	for (int y = 0; y < image.rows; y += blockSize.height)
	{
		for (int x = 0; x < image.cols; x += blockSize.width)
		{
			Rect p = Rect(x, y, blockSize.width, blockSize.height);
			blocks.push_back(p);
		}
	}

	int count = blocks.size();
	thread progressWatcher(ShowProgress, count, image, image2);
	progressWatcher.detach();

	for (list<Rect>::iterator block = blocks.begin(); block != blocks.end(); block++)
	{
		Point mean = Point(block->x + blockSize.width / 2, block->y + blockSize.height / 2);
		Point searchWindowAxis = Point(mean.x - searchWindowSize.width / 2, mean.y - searchWindowSize.height / 2);		
		Rect searchWindow = Rect(searchWindowAxis.x, searchWindowAxis.y, searchWindowSize.width, searchWindowSize.height);

		thread AnalyzeWindow(WindowSearch, Rect(block->x, block->y, block->width, block->height), searchWindow, image, image2);
		AnalyzeWindow.detach();
	}
}

int main(int argc, char** argv) {
	Mat image = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
	Mat image2 = imread(argv[2], CV_LOAD_IMAGE_GRAYSCALE);
	
	FullSearch(image, image2, Rect(0, 0, 64, 64), Rect(0, 0, 128, 128));

	OpenImage(image, "Imagem 1");
	//OpenImage(image2, "Imagem 2");

	waitKey(0);
}


