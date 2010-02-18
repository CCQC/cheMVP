#include "drawinginfo.h"
#include <iostream>

DrawingInfo::DrawingInfo():
        _useFogging(false),
        myXRot(0),
        myYRot(0),
        myZRot(0),
        _foggingScale(DEFAULT_FOGGING_SCALE),
        myDX((int)(DEFAULT_SCENE_SIZE_X/2.0)),
        myDY((int)(DEFAULT_SCENE_SIZE_Y/2.0)),
        myUserDX(0),
        myUserDY(0),
        myMidX((int)(DEFAULT_SCENE_SIZE_X/2.0)),
        myMidY((int)(DEFAULT_SCENE_SIZE_Y/2.0)),
        myWidth((int)(DEFAULT_SCENE_SIZE_X)),
        myHeight((int)(DEFAULT_SCENE_SIZE_Y)),
        myUserScaleFactor(100.0),
        myPerspectiveScale(DEFAULT_PERSPECTIVE_SCALE),
        myMoleculeMaxDimension(1.0),
        myAngToSceneScale(1),
        _maxZ(0.0),
        _minZ(0.0),
        _maxBondZ(0.0),
        _minBondZ(0.0),
        _anglePenWidth(0.2),
        _angleColor(Qt::black),
        _anglePen(Qt::black),
        _anglePrecision(DEFAULT_ANGLE_LABEL_PRECISION),
        _bondColor(Qt::black),
        _bondPrecision(DEFAULT_BOND_LABEL_PRECISION),
        _labelColor(Qt::black),
        _atomLabelFont(DEFAULT_ATOM_LABEL_FONT),
        _atomLineColor(Qt::black),
        _atomTextColor(Qt::black),
        style(Gradient)
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

void DrawingInfo::serialize(QXmlStreamWriter* writer)
{
	writer->writeStartElement("DrawingInfo");	
	writer->writeAttribute("width", QString("%1").arg(myWidth));
	writer->writeAttribute("height", QString("%1").arg(myHeight));
	writer->writeAttribute("xRot", QString("%1").arg(myXRot));
	writer->writeAttribute("yRot", QString("%1").arg(myYRot));
	writer->writeAttribute("zRot", QString("%1").arg(myZRot));
	writer->writeAttribute("midX", QString("%1").arg(myMidX));
	writer->writeAttribute("midY", QString("%1").arg(myMidY));
	writer->writeAttribute("dX", QString("%1").arg(myDX));
	writer->writeAttribute("dY", QString("%1").arg(myDY));
	writer->writeAttribute("userdX", QString("%1").arg(myUserDX));
	writer->writeAttribute("userdY", QString("%1").arg(myUserDY));
	writer->writeAttribute("scale", QString("%1").arg(myUserScaleFactor));
	writer->writeAttribute("perspective", QString("%1").arg(myPerspectiveScale));
	writer->writeAttribute("maxDim", QString("%1").arg(myMoleculeMaxDimension));
	writer->writeAttribute("ang", QString("%1").arg(myAngToSceneScale));
	writer->writeAttribute("maxZ", QString("%1").arg(_maxZ));
	writer->writeAttribute("minZ", QString("%1").arg(_minZ));
	writer->writeAttribute("mazBondZ", QString("%1").arg(_maxBondZ));
	writer->writeAttribute("minBondZ", QString("%1").arg(_minBondZ));
	writer->writeAttribute("fogging", QString("%1").arg(_useFogging));
	writer->writeAttribute("fogScale", QString("%1").arg(_foggingScale));
	writer->writeAttribute("angleWidth", QString("%1").arg(_anglePenWidth));
	writer->writeAttribute("angleColor", QString("%1 %2 %3").arg(_angleColor.red()).arg(_angleColor.green()).arg(_angleColor.blue()));
	writer->writeAttribute("anglePrecision", QString("%1").arg(_anglePrecision));
	writer->writeAttribute("bondColor", QString("%1 %2 %3").arg(_bondColor.red()).arg(_bondColor.green()).arg(_bondColor.blue()));
	writer->writeAttribute("bondPrecision", QString("%1").arg(_bondPrecision));
	writer->writeAttribute("labelColor", QString("%1 %2 %3").arg(_labelColor.red()).arg(_labelColor.green()).arg(_labelColor.blue()));
	writer->writeAttribute("atomLColor", QString("%1 %2 %3").arg(_atomLineColor.red()).arg(_atomLineColor.green()).arg(_atomLineColor.blue()));
	writer->writeAttribute("atomTColor", QString("%1 %2 %3").arg(_atomTextColor.red()).arg(_atomTextColor.green()).arg(_atomTextColor.blue()));
	writer->writeAttribute("style", QString("%1").arg(style));
	writer->writeAttribute("atomFont", QString("%1 %2").arg(_atomLabelFont.family()).arg(_atomLabelFont.pointSize()));
	writer->writeEndElement();
}

DrawingInfo* DrawingInfo::deserialize(QXmlStreamReader* reader)
{
	reader->readNextStartElement();
	Q_ASSERT(reader->name() == "DrawingInfo");
	
	// TODO - Initialize pens, fonts, colors
	DrawingInfo* d = new DrawingInfo();
	QXmlStreamAttributes attr = reader->attributes();
	d->myWidth = attr.value("width").toString().toInt();
	d->myHeight = attr.value("height").toString().toInt();
	d->myXRot = attr.value("xRot").toString().toInt();
	d->myYRot = attr.value("yRot").toString().toInt();	
	d->myZRot = attr.value("zRot").toString().toInt();
	d->myMidX = attr.value("midX").toString().toInt();
	d->myMidY = attr.value("midY").toString().toInt();
	d->myDX = attr.value("dX").toString().toInt();
	d->myDY = attr.value("dY").toString().toInt();
	d->myUserDX = attr.value("userdX").toString().toInt();
	d->myUserDY = attr.value("userdY").toString().toInt();
	d->myUserScaleFactor = attr.value("scale").toString().toInt();
	d->myPerspectiveScale = attr.value("perspective").toString().toInt();
	d->myMoleculeMaxDimension = attr.value("maxDim").toString().toInt();
	d->myAngToSceneScale = attr.value("ang").toString().toInt();
	d->_maxZ = attr.value("maxZ").toString().toInt();
	d->_minZ = attr.value("minZ").toString().toInt();
	d->_maxBondZ = attr.value("maxBondZ").toString().toInt();
	d->_minBondZ = attr.value("minBondZ").toString().toInt();
	d->_useFogging = (attr.value("fogging").toString().toInt() == 1);
	d->_foggingScale = attr.value("fogScale").toString().toInt();
	d->_anglePenWidth = attr.value("anglePenWidth").toString().toInt();
	QString angleColor = attr.value("angleColor").toString();
	d->_anglePrecision = attr.value("anglePrecision").toString().toInt();
	QString bondColor = attr.value("bondColor").toString();
	d->_bondPrecision = attr.value("bondPrecision").toString().toInt();
	QString labelColor = attr.value("labelColor").toString();
	QString atomLColor = attr.value("atomLColor").toString();
	QString atomTColor = attr.value("atomTColor").toString();
	switch(attr.value("style").toString().toInt())
	{
		case 0:  d->style = Gradient; break;
		case 1:  d->style = Simple; break;
		case 2:  d->style = SimpleColored; break;
		case 3:  d->style = HoukMol; break;			
	}
	QString atomFont = attr.value("atomFont").toString();
	reader->skipCurrentElement();
	return d;
}
