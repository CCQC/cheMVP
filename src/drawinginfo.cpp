#include "drawinginfo.h"

DrawingInfo::DrawingInfo():
	myXRot(0.0),
	myYRot(0.0),
	myZRot(0.0),
	myUserScaleFactor(100.0),
	myWidth(DEFAULT_SCENE_SIZE_X),
	myHeight(DEFAULT_SCENE_SIZE_Y),
	myDX(DEFAULT_SCENE_SIZE_X/2.0),
	myDY(DEFAULT_SCENE_SIZE_Y/2.0),
	myPerspectiveScale(0.0015),
	myMoleculeMaxDimension(1.0),
	myAngToSceneScale(1.0)
{
}


DrawingInfo::~DrawingInfo()
{
}


void DrawingInfo::determineScaleFactor()
{
	// To make the the user scale factor run from 0 to 100%, divide by 100, and we need to double the radius
	// of the molecule to find the circumference - the 200 is not a random number!
	myAngToSceneScale = (myWidth>myHeight ? myHeight : myWidth) / (200.0 * myMoleculeMaxDimension);
}