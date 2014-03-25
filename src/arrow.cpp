#include "arrow.h"

DragBox::DragBox(double x, double y, DrawingInfo *info, QGraphicsItem *parent)
		:QGraphicsRectItem(parent),
		hoverOver(false),
		drawingInfo(info),
		// On input, x and y are in scene coordinates
		myDX(x - drawingInfo->dX()),
		myDY(y - drawingInfo->dY())
{
	setFlag(QGraphicsItem::ItemIsSelectable, false);
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setAcceptHoverEvents(true);
	setZValue(1001.0);
	double dimension = 0.1 * drawingInfo->scaleFactor();
	setRect(-dimension/2.0, -dimension/2.0, dimension, dimension);
	setPos(myDX + drawingInfo->dX(), myDY + drawingInfo->dY());
}

void DragBox::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	// A null event to prevent unwanted deselection
	Q_UNUSED(event);
}

void DragBox::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	// A null event to prevent unwanted deselection
	Q_UNUSED(event);
}

void DragBox::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
	Q_UNUSED(event);
	hoverOver = true;
	update();
}

void DragBox::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	Q_UNUSED(event);
	hoverOver = false;
	update();
}

void DragBox::paint(QPainter *painter,
					const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);
	if(!hoverOver)
		return;
	myPen.setWidthF(0.001 * drawingInfo->scaleFactor());
	myPen.setColor(Qt::black);
	painter->setPen(myPen);
	painter->drawRect(rect());
}

Arrow::Arrow(double x, double y, DrawingInfo *info, QGraphicsItem *parent)
		:QGraphicsLineItem(parent),
		drawingInfo(info),
		hoverOver(false)
{
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setAcceptHoverEvents(true);
	setZValue(1000.0);
	myStartBox = new DragBox(x, y, drawingInfo);
	myEndBox   = new DragBox(x, y, drawingInfo);
	setThickness(DEFAULT_ARROW_THICKNESS);
	// So that the width of the line is correct when determining the shape
	myPen.setWidth((int)(3.0*effectiveWidth));
	setPen(myPen);
}

void Arrow::updatePosition(double x, double y)
{
	myEndBox->setDX(x-drawingInfo->dX());
	myEndBox->setDY(y-drawingInfo->dY());
	updatePosition();
}

void Arrow::updatePosition()
{
	double angle = (line().length() == 0.0 ? 0.0 : acos(line().dx() / line().length()));
	double arrowSize = 0.08 * drawingInfo->scaleFactor();
	// Update the arrow position
	myStartBox->setPos(drawingInfo->dX() + myStartBox->dX(),
					   drawingInfo->dY() + myStartBox->dY());
	myEndBox->setPos(drawingInfo->dX() + myEndBox->dX(),
					 drawingInfo->dY() + myEndBox->dY());
	setLine(QLineF(myStartBox->scenePos().x(),
				   myStartBox->scenePos().y(),
				   myEndBox->scenePos().x(),
				   myEndBox->scenePos().y()));

	if(line().dy() >= 0)
		angle = (PI * 2) - angle;

	QPointF arrowP1 = line().p2() - QPointF(sin(angle + PI / 3.0) * arrowSize,
											cos(angle + PI / 3.0) * arrowSize);
	QPointF arrowP2 = line().p2() - QPointF(sin(angle + PI - PI / 3.0) * arrowSize,
											cos(angle + PI - PI / 3.0) * arrowSize);
	arrowHead.clear();
	arrowHead << line().p2() << arrowP1 << arrowP2;

	// Stop the line at the butt of the arrowhead, not the tip
	if(line().length() > arrowSize) {
		setLine(QLineF(line().p1(),
					   line().p2() + QPointF(-cos(angle)*arrowSize*sqrt(3.0)/2.0,
											 sin(angle)*arrowSize*sqrt(3.0)/2.0)));
	}
	// In case the scale factor changed, recompute the thickness
	setThickness(myThickness);
}

void Arrow::paint(QPainter *painter,
				  const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);
	myPen.setWidthF(hoverOver ? 2.0 * effectiveWidth : effectiveWidth);
	myPen.setColor(Qt::black);
	// Draw the line
	painter->setPen(myPen);
	painter->drawLine(line());
	// Now the arrowhead, the brush is thin to make the corners look correct
	painter->setBrush(Qt::black);
	myPen.setWidthF(0.001);
	painter->setPen(myPen);
	painter->drawPolygon(arrowHead);
	if(isSelected()) {
		myPen.setWidthF(hoverOver ? 10.0 * effectiveWidth : effectiveWidth);
		myPen.setColor(SELECTED_COLOR);
		painter->setPen(myPen);
		painter->drawLine(line());
		painter->setBrush(SELECTED_COLOR);
		myPen.setWidthF(0.001);
		painter->setPen(myPen);
		painter->drawPolygon(arrowHead);
	}
}

void Arrow::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	// A null event to prevent unwanted deselection
	Q_UNUSED(event);
}

void Arrow::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	// A null event to prevent unwanted deselection
	Q_UNUSED(event);
}

void Arrow::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
	Q_UNUSED(event);
	hoverOver = true;
	update();
}

void Arrow::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	Q_UNUSED(event);
	hoverOver = false;
	update();
}

void Arrow::serialize(QXmlStreamWriter* writer)
{
	writer->writeStartElement("Arrow");
	writer->writeAttribute("startX", QString("%1").arg(myStartBox->dX()));
	writer->writeAttribute("startY", QString("%1").arg(myStartBox->dY()));
	writer->writeAttribute("endX", QString("%1").arg(myEndBox->dX()));
	writer->writeAttribute("endY", QString("%1").arg(myEndBox->dY()));
	writer->writeAttribute("width", QString("%1").arg(myThickness));
	writer->writeAttribute("effWidth", QString("%1").arg(effectiveWidth));
	writer->writeAttribute("color", QString("%1 %2 %3 %4").arg(myColor.red()).arg(myColor.green()).arg(myColor.blue()).arg(myColor.alpha()));
	writer->writeEndElement();
}

Arrow* Arrow::deserialize(QXmlStreamReader* reader, DrawingInfo* drawingInfo)
{
	Q_ASSERT(reader->isStartElement() && reader->name() == "Arrow");
	QXmlStreamAttributes attr = reader->attributes();
	Arrow* a = new Arrow(0, 0, drawingInfo, NULL);
	a->myStartBox->setDX(attr.value("startX").toString().toDouble());
	a->myStartBox->setDY(attr.value("startY").toString().toDouble());
	a->myEndBox->setDX(attr.value("endX").toString().toDouble());
	a->myEndBox->setDY(attr.value("endY").toString().toDouble());
	a->myThickness = attr.value("width").toString().toDouble();
	a->effectiveWidth = attr.value("effWidth").toString().toDouble();
	QStringList color = attr.value("color").toString().split(" ");
	a->setColor(QColor(color[0].toInt(), color[1].toInt(), color[2].toInt(), color[3].toInt()));
	return a;
}

