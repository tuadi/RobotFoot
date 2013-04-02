#pragma once


#include "ImageProcessing.h"

class ColorFinder
{
public:
	ColorFinder();
	ColorFinder(int hue, int hueTolerance, int saturation, int brightness);
	~ColorFinder();

	CvPoint* getCirclePosition(IplImage* frame);

private:
	static const int MIN_VALUE = 0;
	static const int MAX_VALUE = 255;
	// todo : fichier ini
	static const int WIDTH  = 640;
	static const int HEIGHT = 480;

	int _hue;
	int _hueTolerance;
	int _saturation;
	int _brightness;

	IplImage* _resultFrame;

	void filter(IplImage* frame);
};

