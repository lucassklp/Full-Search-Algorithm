#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace std;
using namespace cv;

class Block
{
public:
	Block() 
	{

	}
	~Block();

private:
	int CornerX, CornerY, FooterX, FooterY;

};

Block::Block()
{
}

Block::~Block()
{
}


void OpenImage(Mat image, string windowName) {
	namedWindow(windowName, WINDOW_AUTOSIZE);
	imshow(windowName, image);
}

void FullSearch(Mat image, Mat image2) {

}

int main(int argc, char** argv) {
	Mat image = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
	Mat image2 = imread(argv[2], CV_LOAD_IMAGE_GRAYSCALE);

	OpenImage(image, "Imagem 1");
	OpenImage(image2, "Imagem 2");

	FullSearch(image, image2);

	waitKey(0);
}


