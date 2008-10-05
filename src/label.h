#ifndef LABEL_H_
#define LABEL_H_

#include <QGraphicsTextItem>
#include <QPen>
#include <QFont>
#include <iostream>
#include "defines.h"
#include "drawinginfo.h"
 class QFocusEvent;
 class QGraphicsItem;
 class QGraphicsScene;
 class QGraphicsSceneMouseEvent;

 class Label : public QGraphicsTextItem
 {
     Q_OBJECT

 public:
     enum LabelType { BondLabelType = UserType + BONDLABELTYPE, AngleLabelType = UserType + ANGLELABELTYPE,
    	 			  TextLabelType = UserType + TEXTLABELTYPE};

     Label(LabelType type, double value, int precision, DrawingInfo *info, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
//     bool sceneEvent(QEvent *event);
     void focusOutEvent (QFocusEvent *event);
//     void keyPressEvent(QKeyEvent * event);
     int type() const { return myType; }
     double dX() {return myDX;}
     double dY() {return myDY;}
     void setDX(double val) {myDX = val;}
     void setDY(double val) {myDY = val;}
     void setPrecision(int val) {myPrecision = val; updateLabel();}
     void setFontSize(int val) {myFontSize = val; updateFontSize();}
     int fontSize() const {return myFontSize;}
//     void setFont(const QString &font) {myFont.setFamily(font);}
//     bool eventFilter(QObject* object, QEvent* event);
      
 signals:
//     void lostFocus(Label *item);
     void selectedChange(QGraphicsItem *item);
 public slots:
 	void updateFontSize();
 protected:
	 void keyPressEvent(QKeyEvent *event);
//	 void keyReleaseEvent(QKeyEvent *event);
     void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
     void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
     void mousePressEvent(QGraphicsSceneMouseEvent *event);

 private:
	 LabelType myType;
	 QString myString;
	 void updateLabel();
	 QColor myColor;
	 int    myFontSize;
	 double myDX;
	 double myDY;
	 double myValue;
	 int myPrecision;
	 DrawingInfo *myDrawingInfo;
 };

#endif /*LABEL_H_*/
