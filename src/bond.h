#ifndef BOND_H_
#define BOND_H_

#include <QtGui>
#include <QGraphicsItem>
#include <cmath>

#include "drawinginfo.h"
#include "atom.h"
#include "defines.h"
#include "label.h"

class Bond : public QGraphicsLineItem
{
public:
	enum { Type = UserType + BONDTYPE};
	int type() const {return Type;}

    Bond(Atom *startAtom, Atom *endAtom, DrawingInfo *drawingInfo, QGraphicsItem *parent = 0);
    
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);

    void setColor(const QColor &color)     { myColor = color; }
    void setThickness(const double val) {myThickness = val;}
    void toggleDashing();
    void toggleLabel();
    void updatePosition();
    Label* label() {return myLabel;};
    Atom *startAtom() const { return myStartAtom; }
    Atom *endAtom() const { return myEndAtom; }
    double length() const { return myLength; } 
    double thickness() const {return myThickness;}
    void setAcceptsHovers(bool arg) {if(!arg) hoverOver = false; setAcceptsHoverEvents(arg);}

protected:
	void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
    Atom *myStartAtom;
    Atom *myEndAtom;
    DrawingInfo *drawingInfo;
    double myThickness;
    double effectiveWidth;
    double myLength;
    QColor myColor;
    bool hoverOver;
    bool dashedLine;
    QPen myPen;
    Label *myLabel;
    double computeLength();
};

#endif /*BOND_H_*/
