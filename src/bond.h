#ifndef BOND_H_
#define BOND_H_

#include <QGraphicsItem>
#include <QtGui>

#include <cmath>
#include <iostream>

#include "atom.h"
#include "defines.h"
#include "drawinginfo.h"
#include "label.h"

class Bond : public QGraphicsLineItem
{
  private:
    void generateDashedPen()
    {
        myPen.setStyle(Qt::CustomDashLine);
        QVector<qreal> dashes;
        qreal space = 3;
        qreal dash = 2;
        dashes << dash << space;
        myPen.setDashPattern(dashes);
    }

  public:
    enum { Type = UserType + BONDTYPE };
    int type() const
    {
        return Type;
    }

    Bond(Atom *startAtom, Atom *endAtom, DrawingInfo *drawingInfo, QGraphicsItem *parent = 0);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    void setColor(const QColor &color)
    {
        _info->setBondColor(color);
    }
    void setThickness(const double val)
    {
        myThickness = val;
    }
    void toggleDashing();
    void toggleLabel();
    void updatePosition();
    Label *label()
    {
        return myLabel;
    };
    Atom *startAtom() const
    {
        return myStartAtom;
    }
    Atom *endAtom() const
    {
        return myEndAtom;
    }
    double length() const
    {
        return myLength;
    }
    double computeMidZ()
    {
        return myStartAtom->z() + myEndAtom->z() / 2.0;
    }
    double thickness() const
    {
        return myThickness;
    }
    void setAcceptsHovers(bool arg)
    {
        if (!arg)
            hoverOver = false;
        setAcceptHoverEvents(arg);
    }
    bool hasLabel()
    {
        return (myLabel != 0);
    }
    bool isDashed() const
    {
        return dashedLine;
    }

    void serialize(QXmlStreamWriter *writer);
    static Bond *
    deserialize(QXmlStreamReader *reader, DrawingInfo *drawingInfo, QList<Atom *> atoms);

  protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    Atom *myStartAtom;
    Atom *myEndAtom;
    DrawingInfo *_info;
    double myThickness;
    double effectiveWidth;
    double myLength;
    bool hoverOver;
    bool dashedLine;
    Label *myLabel;
    QPen myPen;

    double computeLength()
    {
        return Atom::bondLength(myStartAtom, myEndAtom);
    }
};

#endif /*BOND_H_*/
