#pragma once

typedef struct Coordinate {
	int X, Y;
};


class ImageBlock
{
public:
	ImageBlock(Coordinate start, int size);
	~ImageBlock();
	Coordinate leftTop, rightTop, leftBottom, rightBottom, center;
};

ImageBlock::ImageBlock(Coordinate start, int size) {
	this->leftBottom = start;
	this->rightBottom = 

}