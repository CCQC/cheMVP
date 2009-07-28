#ifndef ARROW_H_
#define ARROW_H_

#include <QtGui>
#include <QGraphicsItem>
#include <cmath>
#include <iostream>

#include "drawinginfo.h"
#include "defines.h"

class DragBox : public QGraphicsRectItem
{
public:
    enum { Type = UserType + DRAGBOXTYPE};
    int type() const {return Type;}

    DragBox(double x, double y, DrawingInfo *info, QGraphicsItem *parent = 0);
    
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);

    void setAcceptsHovers(bool arg) {if(!arg) hoverOver = false; setAcceptsHoverEvents(arg);}
    double dX() {return myDX;}
    double dY() {return myDY;}
    void setDX(double val) {myDX = val;}
    void setDY(double val) {myDY = val;}

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    bool hoverOver;
    DrawingInfo *drawingInfo;
    QPen myPen;
    double myDX;
    double myDY;
};

class Arrow : public QGraphicsLineItem
{
public:
    enum { Type = UserType + ARROWTYPE};
    int type() const {return Type;}

    Arrow(double x, double y, DrawingInfo *drawingInfo, QGraphicsItem *parent = 0);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);

    void setColor(const QColor &color)     { myColor = color; }
    void incDX(double val) {myStartBox->setDX(myStartBox->dX()+val); myEndBox->setDX(myEndBox->dX()+val);}
    void incDY(double val) {myStartBox->setDY(myStartBox->dY()+val); myEndBox->setDY(myEndBox->dY()+val);}
    void setThickness(const double val) {myThickness = val; effectiveWidth = drawingInfo->scaleFactor() * val;}
    void updatePosition(double x, double y);
    void updatePosition();
    DragBox* startBox() const { return myStartBox; }
    DragBox* endBox() const { return myEndBox; }
    void setAcceptsHovers(bool arg) {if(!arg) hoverOver = false; setAcceptsHoverEvents(arg);}

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    DragBox *myStartBox;
    DragBox *myEndBox;
    DrawingInfo *drawingInfo;
    QPolygonF arrowHead;
    double myThickness;
    double effectiveWidth;
    QColor myColor;
    bool hoverOver;
    QPen myPen;
};

#endif /*ARROW_H_*/
