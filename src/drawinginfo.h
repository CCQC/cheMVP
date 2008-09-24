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
	// The rotation about the axes
	int myXRot;
	int myYRot;
	int myZRot;
	// The overall translation from the origin of all objects in the scene
	int myDX;
	int myDY;
	// The translation from the center
	int myUserDX;
	int myUserDY;
	// The midpoint of the window
	int myMidX;
	int myMidY;
	// The width and height of the scene
	double myWidth;
	double myHeight;
	// Essentially, just a zoom
	double myUserScaleFactor;
	// The difference in the size of the close and distant atoms
	double myPerspectiveScale;
	double myMoleculeMaxDimension;
	double myAngToSceneScale;
signals:
	void scaleFactorChanged();
};
#endif /*DRAWINGINFO_H_*/
