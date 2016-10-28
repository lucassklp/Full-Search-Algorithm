//
//#define IMAGE_WIDTH 480
//#define IMAGE_HEIGHT 272
//
//#define BLOCK_WIDTH 16 
//#define BLOCK_HEIGHT 16
//
//#define WINDOW_SEARCH_WIDTH 32
//#define WINDOW_SEARCH_HEIGHT 32


//
#define IMAGE_WIDTH 1920
#define IMAGE_HEIGHT 1088

#define BLOCK_WIDTH 64 
#define BLOCK_HEIGHT 64

#define WINDOW_SEARCH_WIDTH 128
#define WINDOW_SEARCH_HEIGHT 128




#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <cmath>
#include <list>
#include <thread>
#include <stdlib.h>

using namespace std;
using namespace cv;



int img1[IMAGE_HEIGHT][IMAGE_WIDTH], img2[IMAGE_HEIGHT][IMAGE_WIDTH], result[IMAGE_HEIGHT][IMAGE_WIDTH];
int total = IMAGE_HEIGHT / BLOCK_HEIGHT * IMAGE_WIDTH / BLOCK_WIDTH, processados = 0;

void OpenImage(Mat image, string windowName);

void MakeImage(Mat image, Mat image2);

void ShowProgress(/*int count, Mat image, Mat image2*/);

int EvaluateMSE(/*int img1[IMAGE_HEIGHT][IMAGE_WIDTH], int img2[IMAGE_HEIGHT][IMAGE_WIDTH],*/ int row, int column, int cx, int cy);

void WindowSearch(/*int img1[IMAGE_HEIGHT][IMAGE_WIDTH], int img2[IMAGE_HEIGHT][IMAGE_WIDTH], int result[IMAGE_HEIGHT][IMAGE_WIDTH],*/ int row, int column, int searchWindowX, int searchWindowY);

void FullSearch(/*int img1[IMAGE_HEIGHT][IMAGE_WIDTH], int img2[IMAGE_HEIGHT][IMAGE_WIDTH], int result[IMAGE_HEIGHT][IMAGE_WIDTH]*/);

void GetGrayscaleMatrix(Mat image, int img[][IMAGE_WIDTH]);

void AddToResult(int row, int column, int bestx, int besty);

int main(/*int argc, char** argv*/) 
{

	Mat image = imread("C:\\blue_sky\\0mini.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	Mat image2 = imread("C:\\blue_sky\\1mini.bmp", CV_LOAD_IMAGE_GRAYSCALE);



	GetGrayscaleMatrix(image, img1);
	GetGrayscaleMatrix(image2, img2);


	FullSearch(/*img1, img2, result*/);

}


void FullSearch(/*int img1[IMAGE_HEIGHT][IMAGE_WIDTH], int img2[IMAGE_HEIGHT][IMAGE_WIDTH], int result[IMAGE_HEIGHT][IMAGE_WIDTH]*/) {






	for (int y = 0; y < IMAGE_HEIGHT; y += BLOCK_HEIGHT)
	{
		for (int x = 0; x < IMAGE_WIDTH; x += BLOCK_WIDTH)
		{

			//WindowSearch(x, y, ((x + BLOCK_WIDTH / 2) - WINDOW_SEARCH_WIDTH / 2), ((y + BLOCK_HEIGHT / 2) - WINDOW_SEARCH_HEIGHT / 2));
			thread AnalyzeWindow(WindowSearch, /*img1, img2, result,*/ x, y, ((x + BLOCK_WIDTH / 2) - WINDOW_SEARCH_WIDTH / 2), ((y + BLOCK_HEIGHT / 2) - WINDOW_SEARCH_HEIGHT / 2));
			AnalyzeWindow.detach();
			//thread Watcher(ShowProgress);
			//Watcher.join();
		}
	}



}

void WindowSearch(/*int img1[IMAGE_HEIGHT][IMAGE_WIDTH], int img2[IMAGE_HEIGHT][IMAGE_WIDTH], int result[IMAGE_HEIGHT][IMAGE_WIDTH],*/ int row, int column, int searchWindowX, int searchWindowY) {

	int minorValue = 99999999;
	int bestx, besty;

	for (int y = searchWindowY; y < searchWindowY + WINDOW_SEARCH_HEIGHT; y++)
	{
		if (y >= 0 && y < IMAGE_HEIGHT - BLOCK_HEIGHT)
		{
			for (int x = searchWindowX; x < searchWindowY + WINDOW_SEARCH_WIDTH; x++)
			{
				if (x >= 0 && x < IMAGE_WIDTH - BLOCK_WIDTH)
				{
					int SADValue = EvaluateMSE(/*img1, img2,*/ row, column, x, y);

					if (SADValue < minorValue) 
					{
						minorValue = SADValue;
						bestx = x;
						besty = y;
					}
				}
			}
		}
	}

	//AddToResult(row, column, bestx, besty);
	cout << ++processados << "/" << total<< endl;
	//++processados;
	//ShowProgress();
}

void AddToResult(int row, int column, int bestx, int besty) {
	for (int x = row; x < row + BLOCK_HEIGHT; x++)
	{
		for (int y = column; y < BLOCK_WIDTH; y++)
		{
			result[row + x][column + y] = img1[bestx + x][besty + y];
		}
	}
}


void OpenImage(Mat image, string windowName) {
	namedWindow(windowName, WINDOW_AUTOSIZE);
	imshow(windowName, image);
}


//void MakeImage(Mat image, Mat image2) {
//	int width = image.cols;
//	int height = image.rows;
//
//
//	//Mat resultImage(width, height, image.type());
//
//	for (list<pair<Rect, Rect>>::iterator match = matchingBlocks.begin(); match != matchingBlocks.end(); match++)
//	{
//		Mat matchBlock = image(match->first);
//		Mat roi = image2(match->second);
//		matchBlock.copyTo(roi);
//	}
//
//	imwrite("result.jpg", image2);
//
//
//	OpenImage(image2, "Final");
//	waitKey(0);
//
//}


void ShowProgress() {

	while (total != processados)
	{
		int x = processados * 100 / total;
		system("cls");
		std::cout << x << "%";
	}
}


//#define IMAGE_WIDTH 1920
//#define IMAGE_HEIGHT 1088
//
//#define BLOCK_WIDTH 64 
//#define BLOCK_HEIGHT 64 
//
//#define WINDOW_SEARCH_WIDTH 128
//#define WINDOW_SEARCH_HEIGHT 128
int EvaluateMSE(/*int img1[IMAGE_HEIGHT][IMAGE_WIDTH], int img2[IMAGE_HEIGHT][IMAGE_WIDTH], */int row, int column, int cx, int cy) {

	int SAD = 0;

	for (int y = column; y < column + BLOCK_WIDTH; y++)
	{
		for (int x = row; x < row + BLOCK_HEIGHT; x++)
		{
			int incrementX = x - row;
			int incrementY = y - column;


			//Point referencialPoint(cx + incrementX, referencialBlock.y + incrementY);
			//Point currentPoint(x, y);

			//int img1 = Image1.at<uchar>(referencialPoint);
			//int img2 = Image2.at<uchar>(currentPoint);



			int valueimg1 = img1[cx + incrementX][cy + incrementY];
			int valueimg2 = img2[x][y];


			int difference = pow(valueimg2 - valueimg1, 2);
			SAD += difference;
		}
	}


	return (SAD);
}







void GetGrayscaleMatrix(Mat image, int img[][IMAGE_WIDTH]) {

	for (int row = 0; row < image.rows; row++)
	{
		for (int column = 0; column < image.cols; column++)
		{
			img[row][column] = (int)image.at<uchar>(Point(column, row));
		}
	}
}
