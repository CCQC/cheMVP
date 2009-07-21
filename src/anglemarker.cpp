#include "anglemarker.h"
 
AngleMarker::AngleMarker(DrawingInfo *info, QGraphicsItem *parent)
	:QGraphicsPathItem(parent),
	drawingInfo(info),
    penWidth(0.2),
	myColor(Qt::black),
	hoverOver(false),
	myPen(Qt::black)
{
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setAcceptsHoverEvents(true);
	effectiveWidth = 0.025 * drawingInfo->scaleFactor();
	myPen.setWidthF(hoverOver ? 1.5*effectiveWidth : effectiveWidth); 	
}

void AngleMarker::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	// A null event to prevent unwanted deselection
	Q_UNUSED(event);
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
	Q_UNUSED(option);
	Q_UNUSED(widget);
	myPen.setWidthF(hoverOver ? 1.5*effectiveWidth : effectiveWidth); 	
 	myPen.setColor(Qt::black);
	painter->setPen(myPen);
 	painter->drawPath(path());
 	if(isSelected()) {
	 	myPen.setColor(SELECTED_COLOR);
		painter->setPen(myPen);
        painter->drawPath(path());        	
	}
}