#ifndef DRAWINGINFO_H_
#define DRAWINGINFO_H_

#include <QObject>
#include "defines.h"

class DrawingInfo: public QObject
{
	
	Q_OBJECT
	
public:
	DrawingInfo();
	~DrawingInfo();
	int xRot()		{return myXRot;}
	int yRot()		{return myYRot;}
	int zRot()		{return myZRot;}
	void incDX(double val) { myUserDX += val; myDX = myMidX + myUserDX;}
	void incDY(double val) { myUserDY += val; myDY = myMidY + myUserDY;}
	void setXRot(int val)	{ myXRot = val;}
	void setYRot(int val)	{ myYRot = val;}
	void setZRot(int val)	{ myZRot = val;}
	void setWidth(double val) { myWidth = val; myMidX = val/2; myDX = myMidX + myUserDX;}
	void setHeight(double val) { myHeight = val; myMidY = val/2; myDY = myMidY + myUserDY;}
	void setMoleculeMaxDimension(double val) {myMoleculeMaxDimension = val;}
	double dX() const   {return myDX;}
	double dY()	const	{return myDY;}
	double moleculeMaxDimension() const {return myMoleculeMaxDimension;}
	double scaleFactor() const {return myUserScaleFactor * myAngToSceneScale;}
	double perspective() const {return myPerspectiveScale;}
	void determineScaleFactor();
    void setZoom(int val) { myUserScaleFactor = (double)val; emit scaleFactorChanged();}
private:
	int myXRot;
	int myYRot;
	int myZRot;
	int myDX;
	int myDY;
	int myUserDX;
	int myUserDY;
	int myMidX;
	int myMidY;
	double myWidth;
	double myHeight;
	double myUserScaleFactor;
	double myPerspectiveScale;
	double myMoleculeMaxDimension;
	double myAngToSceneScale;
signals:
	void scaleFactorChanged();
};
#endif /*DRAWINGINFO_H_*/
