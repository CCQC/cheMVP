#ifndef DRAWINGINFO_H_
#define DRAWINGINFO_H_

// TODO: Forward class declarations
#include <QSettings>
#include <QObject>
#include <QColor>
#include <QPen>
#include <QFont>

#include "defines.h"

class DrawingInfo: public QObject
{
    Q_OBJECT

public:
    DrawingInfo();
    ~DrawingInfo();
    
    enum DrawingStyle {Gradient, Simple, SimpleColored, HoukMol};

    int xRot()		{return myXRot;}
    int yRot()		{return myYRot;}
    int zRot()		{return myZRot;}
    void incDX(double val) { myUserDX += (int)val; myDX = myMidX + myUserDX;}
    void incDY(double val) { myUserDY += (int)val; myDY = myMidY + myUserDY;}
    void setXRot(int val)	{ myXRot = val;}
    void setYRot(int val)	{ myYRot = val;}
    void setZRot(int val)	{ myZRot = val;}
    void setWidth(double val) { myWidth = val; myMidX = (int)val/2; myDX = myMidX + myUserDX;}
    void setHeight(double val) { myHeight = val; myMidY = (int)val/2; myDY = myMidY + myUserDY;}
    void setMoleculeMaxDimension(double val) {myMoleculeMaxDimension = val;}
    void setDrawingStyle(DrawingStyle s) {style = s;}
    DrawingStyle getDrawingStyle() {return style;}
    double dX() const   {return myDX;}
    double dY()	const	{return myDY;}
    double midX() const {return myMidX;}
    double midY() const {return myMidY;}
    double maxZ() const {return _maxZ;}
    double minZ() const {return _minZ;}
    double maxBondZ() const {return _maxBondZ;}
    double minBondZ() const {return _minBondZ;}
    double moleculeMaxDimension() const {return myMoleculeMaxDimension;}
    double scaleFactor() const {return myUserScaleFactor * myAngToSceneScale;}
    double perspective() const {return myPerspectiveScale/10000.0;}
    void determineScaleFactor();
    void setZoom(int val) { myUserScaleFactor = (double)val; emit scaleFactorChanged();}
    void processProjectFile(QSettings &settings, bool saveFile);

    int getFoggingScale() {return _foggingScale;}

    bool getUseFogging() {return _useFogging;}

    void setAnglePenWidth(double v) {_anglePenWidth = v;}
    double getAnglePenWidth() {return _anglePenWidth;}

    void setAngleColor(QColor v) {_angleColor = v;}
    QColor getAngleColor() {return _angleColor;}

    void setAnglePen(QPen v) {_anglePen = v;}
    QPen& getAnglePen() {return _anglePen;}

    void setAnglePrecision(int v) {_anglePrecision = v;}
    double getAnglePrecision() {return _anglePrecision;}

    void setBondColor(QColor v) {_bondColor = v;}
    QColor getBondColor() {return _bondColor;}

    void setBondPrecision(int v) {_bondPrecision = v;}
    int getBondPrecision(){return _bondPrecision;}

    void setLabelColor(QColor v) {_labelColor = v;}
    QColor getLabelColor() {return _labelColor;}

    void setAtomTextColor(QColor v) {_atomTextColor = v;}
    void setAtomLineColor(QColor v) {_atomLineColor = v;}
    void setAtomLabelFont(QString v) {_atomLabelFont.setFamily(v);}

    QColor getAtomTextColor() {return _atomTextColor;}
    QColor getAtomLineColor() {return _atomLineColor;}
    QFont& getAtomLabelFont() {return _atomLabelFont;}

public slots:
    void setFoggingScale(int v) {_foggingScale = v;}
    void setUseFogging(bool v) {_useFogging = v;}
    void setMinZ(double v) {_minZ = v;}
    void setMaxZ(double v) {_maxZ = v;}
    void setMinBondZ(double v) {_minBondZ = v;}
    void setMaxBondZ(double v) {_maxBondZ = v;}

private:
    bool _useFogging;
    // The rotation about the axes
    int myXRot;
    int myYRot;
    int myZRot;
    int _foggingScale;

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
    double _maxZ;
    double _minZ;
    double _maxBondZ;
    double _minBondZ;

    double _anglePenWidth;
    QColor _angleColor;
    QPen _anglePen;
    int _anglePrecision;

    QColor _bondColor;
    int _bondPrecision;

    QColor _labelColor;

    QFont _atomLabelFont;
    QColor _atomLineColor;
    QColor _atomTextColor;

    DrawingStyle style;

signals:
    void scaleFactorChanged();
};
#endif /*DRAWINGINFO_H_*/
