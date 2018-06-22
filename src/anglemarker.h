#ifndef ANGLEMARKER_H_
#define ANGLEMARKER_H_

#include <QGraphicsItem>
#include <QtGui>
#include <cmath>
#include <iostream>

#include "defines.h"
#include "drawinginfo.h"

class AngleMarker : public QGraphicsPathItem
{
  public:
    enum { Type = UserType + ANGLEMARKERTYPE };
    int type() const
    {
        return Type;
    }

    AngleMarker(DrawingInfo *drawingInfo, QGraphicsItem *parent = 0);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    void setOtherMarker(AngleMarker *marker)
    {
        otherMarker = marker;
    }
    void setHover(bool t_f)
    {
        hoverOver = t_f;
    }

    void serialize(QXmlStreamWriter *writer);
    static AngleMarker *deserialize(QXmlStreamReader *reader, DrawingInfo *drawingInfo);

  protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    DrawingInfo *drawingInfo;
    AngleMarker *otherMarker;
    double effectiveWidth;
    QColor myColor;
    bool hoverOver;
    QPen myPen;
};

#endif /*ANGLEMARKER_H_*/
