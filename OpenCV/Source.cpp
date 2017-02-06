//#define IMAGE_WIDTH 480
//#define IMAGE_HEIGHT 272
//#define BLOCK_WIDTH 16 
//#define BLOCK_HEIGHT 16
//#define WINDOW_SEARCH_WIDTH 32
//#define WINDOW_SEARCH_HEIGHT 32

#define IMAGE_WIDTH 1920
#define IMAGE_HEIGHT 1088
#define BLOCK_WIDTH 32 
#define BLOCK_HEIGHT 32
#define WINDOW_SEARCH_WIDTH 64
#define WINDOW_SEARCH_HEIGHT 64

#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <stdlib.h>

using namespace std;
using namespace cv;

int img1[IMAGE_HEIGHT][IMAGE_WIDTH], img2[IMAGE_HEIGHT][IMAGE_WIDTH], result[IMAGE_HEIGHT][IMAGE_WIDTH];

int CalculateSAD(int row, int column, int cx, int cy);
double CalculateMSE(int reference[][IMAGE_WIDTH], int result[][IMAGE_WIDTH]);
double CalculatePSNR(int reference[][IMAGE_WIDTH], int result[][IMAGE_WIDTH]);
void WindowSearch(int row, int column, int searchWindowX, int searchWindowY);
void FullSearch();
void MakeImage(Mat im);
void GetMatrix(Mat image, int img[][IMAGE_WIDTH]);
void AddToResult(int row, int column, int bestx, int besty);

int main() 
{
	int PNSR;
	Mat image = imread("C:\\PIBIC\\0.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	Mat image2 = imread("C:\\PIBIC\\1.bmp", CV_LOAD_IMAGE_GRAYSCALE);

	GetMatrix(image, img1);
	GetMatrix(image2, img2);

	printf("Processando...\n");
	FullSearch();

	MakeImage(image2);
	imwrite("C:\\PIBIC\\R.bmp", image2);

	printf("Finalizado!\n PSNR: %.3f", CalculatePSNR(img2, result));
	getchar();
}

void FullSearch()
{
	for (int row = 0; row < IMAGE_HEIGHT; row += BLOCK_HEIGHT)
	{
		for (int column = 0; column < IMAGE_WIDTH; column += BLOCK_WIDTH)
		{
			WindowSearch(row, column, ((column + BLOCK_WIDTH / 2) - WINDOW_SEARCH_WIDTH / 2), ((row + BLOCK_HEIGHT / 2) - WINDOW_SEARCH_HEIGHT / 2));
		}
	}
}

void WindowSearch(int row, int column, int searchWindowX, int searchWindowY) 
{
	int minorSAD = 99999999;
	int bestx = row, besty = column, SAD;

	for (int y = searchWindowY; y < searchWindowY + WINDOW_SEARCH_HEIGHT; y++)
	{
		if (y >= 0 && y < IMAGE_HEIGHT - BLOCK_HEIGHT)
		{
			for (int x = searchWindowX; x < searchWindowX + WINDOW_SEARCH_WIDTH; x++)
			{
				if (x >= 0 && x < IMAGE_WIDTH - BLOCK_WIDTH)
				{
					SAD = CalculateSAD(row, column, x, y);

					if (SAD < minorSAD) 
					{
						minorSAD = SAD;
						bestx = x;
						besty = y;
					}
				}
			}
		}
	}
	AddToResult(row, column, bestx, besty);
}

void AddToResult(int row, int column, int bestx, int besty) 
{
	for (int y = 0; y < BLOCK_HEIGHT; y++)
	{
		for (int x = 0; x < BLOCK_WIDTH; x++)
		{
			result[row + y][column + x] = img1[besty + y][bestx + x];
		}
	}
}

int CalculateSAD(int row, int column, int cx, int cy) 
{
	int SAD = 0;
	for (int y = row; y < row + BLOCK_HEIGHT; y++)
	{
		for (int x = column; x < column + BLOCK_WIDTH ; x++)
		{
			SAD += abs(img2[y][x] - img1[cy + y - row][cx + x - column]);
		}
	}

	return SAD;
}

double CalculateMSE(int reference[][IMAGE_WIDTH], int result[][IMAGE_WIDTH])
{
	double soma = 0.0;
	double MSE;
	for (int row = 0; row < IMAGE_HEIGHT; row++)
	{
		for (int column = 0; column < IMAGE_WIDTH; column++)
		{
			soma += pow(reference[row][column] - result[row][column], 2);
		}
	}

	MSE = soma / (IMAGE_WIDTH * IMAGE_HEIGHT);

	return MSE;
}

double CalculatePSNR(int reference[][IMAGE_WIDTH], int result[][IMAGE_WIDTH]) 
{
	double MSE = CalculateMSE(reference, result);
	return (20 * log10(255.0)) - (10 * log10(MSE));
}

void GetMatrix(Mat image, int img[][IMAGE_WIDTH]) 
{
	for (int row = 0; row < image.rows; row++)
	{
		for (int column = 0; column < image.cols; column++)
		{
			img[row][column] = (int)image.at<uchar>(Point(column, row));
		}
	}
}

void MakeImage(Mat im) 
{
	for (int row = 0; row < IMAGE_HEIGHT; row++)
	{
		for (int column = 0; column < IMAGE_WIDTH; column++)
		{
			im.at<uchar>(Point(column, row)) = result[row][column];
		}
	}
}