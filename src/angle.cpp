#include "angle.h"

Angle::Angle(Atom *atom1, Atom * atom2, Atom *atom3, DrawingInfo *info, QGraphicsItem *parent)
	:QGraphicsPathItem(parent),
	myStartAtom(atom1),
	myCenterAtom(atom2),
	myEndAtom(atom3),
	_info(info),
	hoverOver(false)
{
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setAcceptHoverEvents(true);
	_info->setAnglePenWidth(0.05 * _info->scaleFactor());
	_info->getAnglePen().setWidthF(hoverOver ? 1.5*_info->getAnglePenWidth() : _info->getAnglePenWidth());
	myValue = computeValue();
	myLabel = new Label(Label::AngleLabelType, myValue, _info);
	// Time to figure out how to make the Angle labels appear correctly
	myMarker1 = new AngleMarker(_info);
	myMarker2 = new AngleMarker(_info);
	// The two halves of the angle marker are linked to each other
	// so they appear as one for selection/hovering events
	myMarker1->setOtherMarker(myMarker2);
	myMarker2->setOtherMarker(myMarker1);
	updatePosition();
}

Angle::~Angle()
{
	delete myLabel;
	delete myMarker1;
	delete myMarker2;
}

void Angle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	// A null event to prevent unwanted deselection
	Q_UNUSED(event);
}

void Angle::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
	Q_UNUSED(event);
	hoverOver = true;
	update();
}

void Angle::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	Q_UNUSED(event);
	hoverOver = false;
	update();
}

void Angle::updatePosition()
{
	_info->getAnglePen().setColor(_info->getAngleColor());
	setPen(_info->getAnglePen());
	// The angle between bond 2 and the vertical
	double dX1   		= myStartAtom->pos().x() - myCenterAtom->pos().x();
	double dY1   		= myStartAtom->pos().y() - myCenterAtom->pos().y();
	double dZ1   		= myStartAtom->zValue() - myCenterAtom->zValue();
	double norm1 		= sqrt(dX1*dX1 + dY1*dY1);
	double r1    		= sqrt(dX1*dX1 + dY1*dY1 + dZ1*dZ1);
	double cosTheta1 	= dZ1/r1;
	double sinTheta1    = sin(acos(cosTheta1));
	double phi1  		= (norm1 == 0.0 ? 0.0 : acos(dY1/norm1));
	if(dX1 < 0.0) phi1  = 2.0*PI - phi1;
	// The angle between bond 2 and the vertical
	double dX3   		= myEndAtom->pos().x() - myCenterAtom->pos().x();
	double dY3   		= myEndAtom->pos().y() - myCenterAtom->pos().y();
	double dZ3  		= myEndAtom->zValue() - myCenterAtom->zValue();
	double norm3 		= sqrt(dX3*dX3 + dY3*dY3);
	double r3    		= sqrt(dX3*dX3 + dY3*dY3 + dZ3*dZ3);
	double cosTheta3 	= dZ3/r3;
	double sinTheta3    = sin(acos(cosTheta3));
	double phi3  		= (norm3 == 0.0 ? 0.0 : acos(dY3/norm3));
	if(dX3 < 0.0) phi3  = 2.0*PI - phi3;

	//double phi2  		= (phi1 + phi3) / 2.0; // Unused variable - HPS
	double radius 		= myCenterAtom->effectiveRadius() + 0.2 * _info->scaleFactor();

	double xRef = myCenterAtom->pos().x();
	double yRef = myCenterAtom->pos().y();
	double zRef = myCenterAtom->zValue();

	// These are the positions where the angle marker arc touches the bonds
	QPointF bond1Pos(xRef + radius*sinTheta1*sin(phi1), yRef + radius*sinTheta1*cos(phi1));
	QPointF bond2Pos(xRef + radius*sinTheta3*sin(phi3), yRef + radius*sinTheta3*cos(phi3));
	double bond1Z   =  zRef + radius * cosTheta1;
	double bond2Z   =  zRef + radius * cosTheta3;

	// The vectors from atom2 to the points startPos and endPos are normalized to radius by construction.  The
	// sum of them is the midpoint, but it must be renormalized to radius
	double scale  = radius/sqrt(pow(bond1Pos.x() + bond2Pos.x() - 2.0*xRef, 2.0)
								+  pow(bond1Pos.y() + bond2Pos.y() - 2.0*yRef, 2.0)
								+  pow(bond1Z       + bond2Z       - 2.0*zRef, 2.0));
	// This is the midpoint on the arc from p1 to p3
	double zMP = zRef + scale*(bond1Z + bond2Z - 2.0*zRef);
	QPointF MP( xRef + scale*(bond1Pos.x() + bond2Pos.x() - 2.0*xRef),
				yRef + scale*(bond1Pos.y() + bond2Pos.y() - 2.0*yRef));

	// Time to account for the angle markers not touching the bonds
	double fraction = atan(ANGLE_MARKER_OFFSET);
	scale  = radius/sqrt(pow(bond1Pos.x() - xRef + fraction * (bond2Pos.x() - xRef), 2.0)
						 +  pow(bond1Pos.y() - yRef + fraction * (bond2Pos.y() - yRef), 2.0)
						 +  pow(bond1Z - zRef       + fraction * (bond2Z - zRef), 2.0));
	QPointF startPos(xRef + scale * (bond1Pos.x() - xRef + fraction * (bond2Pos.x() - xRef)),
					 yRef + scale * (bond1Pos.y() - yRef + fraction * (bond2Pos.y() - yRef)));
	QPointF   endPos(xRef + scale * (bond2Pos.x() - xRef + fraction * (bond1Pos.x() - xRef)),
					 yRef + scale * (bond2Pos.y() - yRef + fraction * (bond1Pos.y() - yRef)));
	double startZ = zRef + scale * (bond1Z - zRef + fraction * (bond2Z - zRef));
	double endZ   = zRef + scale * (bond2Z - zRef + fraction * (bond1Z - zRef));


	// Redefine this to be the normalization for the quarter and three-quarter point vectors
	scale  = radius/sqrt(pow(MP.x() + startPos.x() - 2.0*xRef, 2.0)
						 +  pow(MP.y() + startPos.y() - 2.0*yRef, 2.0)
						 +  pow(zMP    + startZ       - 2.0*zRef, 2.0));

	// This will be the quarter point along the arc
	QPointF QP1(xRef + scale*(startPos.x() + MP.x() - 2.0*xRef),
				yRef + scale*(startPos.y() + MP.y() - 2.0*yRef));
	// This will be the three-quarter point along the arc
	QPointF QP3(xRef + scale*(endPos.x() + MP.x() - 2.0*xRef),
				yRef + scale*(endPos.y() + MP.y() - 2.0*yRef));
	myMarker1->setZValue(startZ);
	myMarker2->setZValue(endZ);

	// This is the interpolation formula to force the curve through QP1
	QPointF guidePoint1 = 2.0*QP1 - 0.5*(startPos + MP);
	// This is the interpolation formula to force the curve through QP3
	QPointF guidePoint3 = 2.0*QP3 - 0.5*(endPos + MP);

	QPainterPath path1(startPos);
	path1.quadTo(guidePoint1, MP);
	QPainterPath path2(endPos);
	path2.quadTo(guidePoint3, MP);

	myMarker1->setPath(path1);
	myMarker2->setPath(path2);

	// A cheap and cheerful guess of the label's position
	// TODO - fix me
	if(myLabel != 0) {
		QPointF labelPos;
		labelPos.setX(xRef + myLabel->dX() + 1.5*scale*(endPos.x() + startPos.x() - 2.0*xRef));
		labelPos.setY(yRef + myLabel->dY() + 1.5*scale*(endPos.y() + startPos.y() - 2.0*yRef));
		myLabel->setPos(labelPos);
	}
}

void Angle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(painter);
	Q_UNUSED(option);
	Q_UNUSED(widget);
}

double Angle::computeValue()
{
	double cosTheta = ((myCenterAtom->x()-myStartAtom->x())*(myCenterAtom->x()-myEndAtom->x())
					   + (myCenterAtom->y()-myStartAtom->y())*(myCenterAtom->y()-myEndAtom->y())
					   + (myCenterAtom->z()-myStartAtom->z())*(myCenterAtom->z()-myEndAtom->z()))
					  / (Atom::bondLength(myCenterAtom, myStartAtom) * Atom::bondLength(myCenterAtom, myEndAtom));
	return acos(cosTheta)*RAD_TO_DEG;
}

void Angle::serialize(QXmlStreamWriter* writer)
{
	writer->writeStartElement("Angle");
	writer->writeAttribute("startAtomID", QString("%1").arg(myStartAtom->ID()));
	writer->writeAttribute("centerAtomID", QString("%1").arg(myCenterAtom->ID()));
	writer->writeAttribute("endAtomID", QString("%1").arg(myEndAtom->ID()));
	writer->writeAttribute("value", QString("%1").arg(myValue));
	myMarker1->serialize(writer);
	myMarker2->serialize(writer);
	myLabel->serialize(writer);
	writer->writeEndElement();
}

Angle* Angle::deserialize(QXmlStreamReader* reader, DrawingInfo* drawingInfo, QList<Atom*> atoms, QGraphicsScene* scene)
{
	Q_ASSERT(reader->isStartElement() && reader->name() == "Angle");
	QXmlStreamAttributes attr = reader->attributes();
	Atom* start;
	Atom* center;
	Atom* end;

	int s = attr.value("startAtomID").toString().toInt();
	int c = attr.value("centerAtomID").toString().toInt();
	int e = attr.value("endAtomID").toString().toInt();
	foreach(Atom* a, atoms)
	{
		if(a->ID() == s)
			start = a;
		if(a->ID() == e)
			end = a;
		if(a->ID() == c)
			center = a;
	}
	Angle* a = new Angle(start, center, end, drawingInfo, NULL);
	a->myValue = attr.value("value").toString().toDouble();
	reader->readNextStartElement();
	a->myMarker1 = AngleMarker::deserialize(reader, drawingInfo);
	reader->skipCurrentElement();
	reader->readNextStartElement();
	a->myMarker2 = AngleMarker::deserialize(reader, drawingInfo);
	reader->skipCurrentElement();
	reader->readNextStartElement();
	a->myLabel = Label::deserialize(reader, drawingInfo, scene);
	reader->skipCurrentElement();
	a->myMarker1->setOtherMarker(a->myMarker2);
	a->myMarker2->setOtherMarker(a->myMarker1);
	return a;
}
