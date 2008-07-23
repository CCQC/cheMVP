#ifndef DRAWINGINFO_H_
#define DRAWINGINFO_H_

#include <QTransform>
#include "defines.h"

class DrawingInfo: public QTransform
{
public:
	DrawingInfo();
	~DrawingInfo();
	int xRot()		{return myXRot;}
	int yRot()		{return myYRot;}
	int zRot()		{return myZRot;}
	void incDX(double val) { myDX += val;}
	void incDY(double val) { myDY += val;}
	void setXRot(int val)	{ myXRot = val;}
	void setYRot(int val)	{ myYRot = val;}
	void setZRot(int val)	{ myZRot = val;}
	void setWidth(double val) { myWidth = val; }
	void setHeight(double val) { myHeight = val; }
	void setMoleculeMaxDimension(double val) {myMoleculeMaxDimension = val;}
	double dX() const		{return myDX;}
	double dY()	const	{return myDY;}
	double moleculeMaxDimension() const {return myMoleculeMaxDimension;}
	double scaleFactor() const {return myUserScaleFactor * myAngToSceneScale;}
	double perspective() const {return myPerspectiveScale;}
	void determineScaleFactor();
private:
	int myXRot;
	int myYRot;
	int myZRot;
	int myDX;
	int myDY;
	double myWidth;
	double myHeight;
	double myUserScaleFactor;
	double myPerspectiveScale;
	double myMoleculeMaxDimension;
	double myAngToSceneScale;
};
#endif /*DRAWINGINFO_H_*/
