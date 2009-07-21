#include "drawinginfo.h"

DrawingInfo::DrawingInfo():
	myXRot(0.0),
	myYRot(0.0),
	myZRot(0.0),
	myDX(DEFAULT_SCENE_SIZE_X/2.0),
	myDY(DEFAULT_SCENE_SIZE_Y/2.0),
	myUserDX(0),
	myUserDY(0),
	myMidX(DEFAULT_SCENE_SIZE_X/2.0),
	myMidY(DEFAULT_SCENE_SIZE_Y/2.0),
	myWidth(DEFAULT_SCENE_SIZE_X),
	myHeight(DEFAULT_SCENE_SIZE_Y),
	myUserScaleFactor(100.0),
	myPerspectiveScale(DEFAULT_PERSPECTIVE_SCALE),
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
	emit scaleFactorChanged();
}

void DrawingInfo::processProjectFile(QSettings &settings, bool saveFile)
{
	if(saveFile){
		settings.setValue("XRot", myXRot);
		settings.setValue("YRot", myYRot);
		settings.setValue("ZRot", myZRot);
		settings.setValue("Zoom", myUserScaleFactor);
		settings.setValue("sceneWidth", myWidth);
		settings.setValue("sceneHeight", myHeight);
		settings.setValue("MidX", myMidX);
		settings.setValue("MidY", myMidY);
		settings.setValue("TotalDX", myDX);
		settings.setValue("TotalDY", myDY);
		settings.setValue("UserDX", myUserDX);
		settings.setValue("UserDY", myUserDY);
		settings.setValue("Perspective", myPerspectiveScale);
		settings.setValue("MaxDimension", myMoleculeMaxDimension);
		settings.setValue("SceneScale", myAngToSceneScale);
	}else{
		myXRot = settings.value("XRot", 0).toInt();
		myYRot = settings.value("YRot", 0).toInt();
		myZRot = settings.value("ZRot", 0).toInt();
		myUserScaleFactor = settings.value("Zoom", 100.0).toDouble();
		myWidth = settings.value("sceneWidth", DEFAULT_SCENE_SIZE_X).toDouble();
		myHeight = settings.value("sceneHeight", DEFAULT_SCENE_SIZE_Y).toDouble();
		myMidX = settings.value("MidX", DEFAULT_SCENE_SIZE_X/2.0).toDouble();
		myMidY = settings.value("MidY", DEFAULT_SCENE_SIZE_Y/2.0).toDouble();
		myDX = settings.value("TotalDX", DEFAULT_SCENE_SIZE_X/2.0).toDouble();
		myDY = settings.value("TotalDY", DEFAULT_SCENE_SIZE_Y/2.0).toDouble();
		myDX = settings.value("UserDX", 0.0).toDouble();
		myDY = settings.value("UserDY", 0.0).toDouble();
		myPerspectiveScale = settings.value("Perspective", DEFAULT_PERSPECTIVE_SCALE).toDouble();
		myMoleculeMaxDimension = settings.value("MaxDimension", 0.0).toDouble();
		myAngToSceneScale = settings.value("SceneScale", 0.0).toDouble();		
	}
}