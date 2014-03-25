#include "anglemarker.h"

AngleMarker::AngleMarker(DrawingInfo *info, QGraphicsItem *parent)
		:QGraphicsPathItem(parent),
		drawingInfo(info),
		myColor(Qt::black),
		hoverOver(false),
		myPen(Qt::black)
{
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setAcceptHoverEvents(true);
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

void AngleMarker::serialize(QXmlStreamWriter* writer)
{
	writer->writeStartElement("AngleMarker");
	writer->writeAttribute("effWidth", QString("%1").arg(effectiveWidth));
	writer->writeAttribute("color", QString("%1 %2 %3 %4").arg(myColor.red()).arg(myColor.green()).arg(myColor.blue()).arg(myColor.alpha()));
	writer->writeEndElement();
}

AngleMarker* AngleMarker::deserialize(QXmlStreamReader* reader, DrawingInfo* drawingInfo)
{
	Q_ASSERT(reader->isStartElement() && reader->name() == "AngleMarker");
	AngleMarker* m = new AngleMarker(drawingInfo, NULL);
	m->effectiveWidth = reader->attributes().value("effWidth").toString().toDouble();
	QStringList color = reader->attributes().value("color").toString().split(" ");
	m->myColor = QColor(color[0].toInt(), color[1].toInt(), color[2].toInt(), color[3].toInt());
	return m;
}
