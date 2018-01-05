#ifndef ANGLE_H_
#define ANGLE_H_

#include <QtGui>
#include <QGraphicsItem>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <cmath>
#include <iostream>

#include "drawinginfo.h"
#include "anglemarker.h"
#include "atom.h"
#include "defines.h"
#include "label.h"

class Angle : public QGraphicsPathItem
{
public:
    enum {Type = UserType + ANGLETYPE};
    int type() const {return Type;}

    Angle(Atom *startAtom, Atom *centerAtom, Atom *endAtom, DrawingInfo *drawingInfo,
          QGraphicsItem *parent = 0);
    ~Angle();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);
    void toggleLabel();
    Label* label() {return myLabel;};
    void updatePosition();
    Atom *startAtom() const { return myStartAtom; }
    Atom *centerAtom() const {return myCenterAtom;}
    Atom *endAtom() const { return myEndAtom; }
    AngleMarker *marker1() {return myMarker1;}
    AngleMarker *marker2() {return myMarker2;}

    void serialize(QXmlStreamWriter* writer);
    static Angle* deserialize(QXmlStreamReader* reader, DrawingInfo* drawingInfo, QList<Atom*> atoms, QGraphicsScene* scene);

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    Atom* myStartAtom;
    Atom* myCenterAtom;
    Atom* myEndAtom;
    AngleMarker* myMarker1;
    AngleMarker* myMarker2;
    DrawingInfo* _info;
    double myValue;
    bool hoverOver;
    Label *myLabel;
    double computeValue();
};

#endif /*ANGLE_H_*/
