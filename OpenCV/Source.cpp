#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <list>
#include <limits>
#include <cmath>
#include <thread>
#include <stdlib.h>

using namespace std;
using namespace cv;

list<Rect> matchingBlocks;
bool isFinished = false;

void OpenImage(Mat image, string windowName) {
	namedWindow(windowName, WINDOW_AUTOSIZE);
	imshow(windowName, image);
}	

void ShowProgress(int count) {
	while (!isFinished)
	{
		int x = matchingBlocks.size() * 100 / count;
		system("cls");
		std::cout << x << "%";

		if (matchingBlocks.size() == count) {
			isFinished = true;
		}
	}
}

int EvaluateSAD(Rect currentBlock, Rect referencialBlock, Mat Image1, Mat Image2) {
	
	int SAD = 0;

	for (int i = currentBlock.x; i < currentBlock.x + currentBlock.width; i++)
	{
		for (int j = currentBlock.y; j < currentBlock.y + currentBlock.height; j++)
		{
			int incrementX = i - currentBlock.x;
			int incrementY = j - currentBlock.y;
			int img1 = Image1.at<uchar>(referencialBlock.x + incrementX, referencialBlock.y + incrementY);
			int img2 = Image2.at<uchar>(i, j);



			int difference = std::pow((img2 - img1), 2);
			SAD += difference;
		}
	}
	

	return SAD;
}

void WindowSearch(Rect block, Rect blockSize, Rect searchWindowSize, Rect searchWindow, Mat image, Mat image2) {

	int minorValue = std::numeric_limits<int>::max();
	Rect matchingBlock;

	for (int i = searchWindow.x; i < searchWindow.x + searchWindowSize.width; i++)
	{
		if (i >= 0 && i < image.rows - block.height)
		{
			for (int j = searchWindow.y; j < searchWindow.y + searchWindowSize.height; j++)
			{
				if (j >= 0 && j < image.cols - block.width)
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

	matchingBlocks.push_back(matchingBlock);
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

	Mat resultImage = Mat(image.rows, image.cols, image.type());
	

	int count = blocks.size();
	thread progressWatcher(ShowProgress, count);
	progressWatcher.detach();
	
	for (list<Rect>::iterator block = blocks.begin(); block != blocks.end(); block++)
	{
		Point mean = Point(block->x + blockSize.width / 2, block->y + blockSize.height / 2);
		Point searchWindowAxis = Point(mean.x - searchWindowSize.width / 2, mean.y - searchWindowSize.height / 2);		
		Rect searchWindow = Rect(searchWindowAxis.x, searchWindowAxis.y, searchWindowSize.width, searchWindowSize.height);

		thread AnalyzeWindow(WindowSearch, Rect(block->x, block->y, block->width, block->height), blockSize, searchWindowSize, searchWindow, image, image2);
		AnalyzeWindow.detach();
	}

}




int main(int argc, char** argv) {
	Mat image = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
	Mat image2 = imread(argv[2], CV_LOAD_IMAGE_GRAYSCALE);
	
	FullSearch(image, image2, Rect(0, 0, 32, 32), Rect(0, 0, 128, 128));

	OpenImage(image, "Imagem 1");
	OpenImage(image2, "Imagem 2");



	waitKey(0);
}


