#include "anglemarker.h"
#include<iostream>
 
 AngleMarker::AngleMarker(DrawingInfo *info, QGraphicsItem *parent)
     : QGraphicsPathItem(parent),
     drawingInfo(info),
     myColor(Qt::black),
     hoverOver(false),
     myPen(Qt::black),
     penWidth(0.2)
 {
     setFlag(QGraphicsItem::ItemIsSelectable, true);
	 setAcceptsHoverEvents(true);
	 effectiveWidth = 0.025 * drawingInfo->scaleFactor();
	 myPen.setWidthF(hoverOver ? 1.5*effectiveWidth : effectiveWidth); 	
 }


 void AngleMarker::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
 {
	// A null event to prevent unwanted deselection
 }

 
  
 void AngleMarker::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
 {
	 Q_UNUSED(event);
	 hoverOver = true;
	 otherMarker->setHover(true);
	 update();
 }

 
 void AngleMarker::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
 {
	 Q_UNUSED(event);
	 hoverOver = false;
	 otherMarker->setHover(false);
	 update();
 }

 
 
 void AngleMarker::paint(QPainter *painter,
            const QStyleOptionGraphicsItem *option, QWidget *widget)
 {
	myPen.setWidthF(hoverOver ? 1.5*effectiveWidth : effectiveWidth); 	
 	myPen.setColor(Qt::black);
	painter->setPen(myPen);
 	painter->drawPath(path());
 	if(isSelected()){
	 	myPen.setColor(SELECTED_COLOR);
		painter->setPen(myPen);
        painter->drawPath(path());        	
    }
 }

 