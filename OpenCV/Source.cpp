#define IMAGE_WIDTH 480
#define IMAGE_HEIGHT 272
#define BLOCK_WIDTH 16 
#define BLOCK_HEIGHT 16
#define WINDOW_SEARCH_WIDTH 32
#define WINDOW_SEARCH_HEIGHT 32

//#define IMAGE_WIDTH 1920
//#define IMAGE_HEIGHT 1088
//#define BLOCK_WIDTH 64 
//#define BLOCK_HEIGHT 64
//#define WINDOW_SEARCH_WIDTH 128
//#define WINDOW_SEARCH_HEIGHT 128

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
//static int total = IMAGE_HEIGHT / BLOCK_HEIGHT * IMAGE_WIDTH / BLOCK_WIDTH;

int EvaluateSAD(int row, int column, int cx, int cy);

void WindowSearch(int row, int column, int searchWindowX, int searchWindowY);

void FullSearch();

Mat MakeImage();

void GetGrayscaleMatrix(Mat image, int img[][IMAGE_WIDTH]);

void AddToResult(int row, int column, int bestx, int besty);

int main() 
{

	Mat image = imread("C:\\PIBIC\\0mini.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	Mat image2 = imread("C:\\PIBIC\\1mini.bmp", CV_LOAD_IMAGE_GRAYSCALE);

	GetGrayscaleMatrix(image, img1);
	GetGrayscaleMatrix(image2, img2);

	FullSearch();

	Mat r = MakeImage();
	imwrite("C:\\PIBIC\\R.bmp", r);

}


void FullSearch()
{
	for (int y = 0; y < IMAGE_HEIGHT; y += BLOCK_HEIGHT)
	{
		for (int x = 0; x < IMAGE_WIDTH; x += BLOCK_WIDTH)
		{
			WindowSearch(x, y, ((x + BLOCK_WIDTH / 2) - WINDOW_SEARCH_WIDTH / 2), ((y + BLOCK_HEIGHT / 2) - WINDOW_SEARCH_HEIGHT / 2));
		}
	}
}

void WindowSearch(int row, int column, int searchWindowX, int searchWindowY) 
{
	int minorValue = 99999999;
	int bestx = searchWindowX, besty = searchWindowY, SADValue;

	for (int y = searchWindowY; y < searchWindowY + WINDOW_SEARCH_HEIGHT; y++)
	{
		if (y >= 0 && y < IMAGE_HEIGHT - BLOCK_HEIGHT)
		{
			for (int x = searchWindowX; x < searchWindowY + WINDOW_SEARCH_WIDTH; x++)
			{
				if (x >= 0 && x < IMAGE_WIDTH - BLOCK_WIDTH)
				{
					SADValue = EvaluateSAD(row, column, x, y);

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
	AddToResult(searchWindowY, searchWindowX, bestx, besty);
}

void AddToResult(int row, int column, int bestx, int besty) 
{
	for (int x = row; x < row + BLOCK_HEIGHT; x++)
	{
		for (int y = column; y < BLOCK_WIDTH; y++)
		{
			result[row + x][column + y] = img1[bestx + x][besty + y];
		}
	}
}


int EvaluateSAD(int row, int column, int cx, int cy) 
{
	int SAD = 0;

	for (int y = column; y < column + BLOCK_WIDTH; y++)
	{
		for (int x = row; x < row + BLOCK_HEIGHT; x++)
		{
			SAD += img2[x][y] - img1[cx + x - row][cy + y - column];
		}
	}

	return SAD;
}


void GetGrayscaleMatrix(Mat image, int img[][IMAGE_WIDTH]) 
{
	for (int row = 0; row < image.rows; row++)
	{
		for (int column = 0; column < image.cols; column++)
		{
			img[row][column] = (int)image.at<uchar>(Point(column, row));
		}
	}
}

Mat MakeImage() {
	Mat image(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC3);

	for (int row = 0; row < image.rows; row++)
	{
		for (int column = 0; column < image.cols; column++)
		{
			image.at<uchar>(Point(column, row)) = result[row][column];
		}
	}

	return image;
}