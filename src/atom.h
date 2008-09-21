#ifndef AtomDrawing_H
#define AtomDrawing_H

#include <QGraphicsItem>

#include <map>
#include <QString>
#include "error.h"
#include "defines.h"
#include "drawinginfo.h"

 class Atom : public QGraphicsEllipseItem
 {
 public:
	 enum { Type = UserType + ATOMTYPE};
	 enum DrawingStyle {Simple, SimpleColored, HoukMol, Gradient};
	 enum FontSizeStyle {SmallLabel, LargeLabel};
     Atom(QString element, DrawingInfo *drawingInfo, QGraphicsItem *parent = 0);

     QRectF boundingRect() const;
     void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
     int type() const {return Type;}
     int fontSize() const {return myFontSize;}
     double radius() const {return myRadius;}
     double mass() const {return myMass;}
     double x() const {return myX;}
     double y() const {return myY;}
     double z() const {return myZ;}
     double scaleFactor() const {return myScaleFactor;}
     double effectiveRadius() const {return myEffectiveRadius;}
     QString label() {return ( myLabel + 
                              (myLabelSubscript.size() ? "_" + myLabelSubscript : "") +
                              (myLabelSuperscript.size() ? "^" + myLabelSuperscript : ""));}
     const QFont& labelFont() {return myLabelFont;}
     void computeRadius();
     void setLabelSubscript(const QString &string) {myLabelSubscript = string;}
     void setLabelFontSize(int val) {myFontSize = val; myLabelFont.setPointSizeF(double(val)*myEffectiveRadius/20.0);}
     void setLabelFont(const QString &font) {myLabelFont.setFamily(font);}
     void setScaleFactor(double val) {myScaleFactor = val;}
     void setX(double val) {myX = val;}
     void setY(double val) {myY = val;}
     void setZ(double val) {myZ = val;}
     void setLabel(const QString &text);
     void setAcceptsHovers(bool arg) {if(!arg) hoverOver = false; setAcceptsHoverEvents(arg);}
     void setDrawingStyle(DrawingStyle style);
     void setFontSizeStyle(FontSizeStyle style);
 protected:
     static std::map<QString, double> labelToVdwRadius;
     static std::map<QString, double> labelToMass;
     static std::map<QString, QColor> labelToColor;
	 static void fillLabelToVdwRadiusMap();
	 static void fillLabelToMassMap();
	 static void fillLabelToColorMap();

	 void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
     void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

	 double myEffectiveRadius;
	 DrawingStyle myDrawingStyle;
	 FontSizeStyle myFontSizeStyle;
	 double myMass;
	 double myRadius;
	 double myX;
	 double myY;
	 double myZ;
	 double myScaleFactor;
 	 QString myLabel;
 	 QString mySymbol;
 	 QString myLabelSubscript;
     QString myLabelSuperscript;
 	 int myFontSize;
 	 bool hoverOver;
 	 QFont myLabelFont;
     QColor line_color; 
     QColor text_color;
     QColor fill_color;
     DrawingInfo *drawingInfo;
 };

#endif
