#include "bond.h"
#include<iostream>
 
 Bond::Bond(Atom *atom1, Atom *atom2, DrawingInfo *info, QGraphicsItem *parent)
     : QGraphicsLineItem(parent),
     myStartAtom(atom1),
     myEndAtom(atom2),
     drawingInfo(info),
     myColor(Qt::black),
     hoverOver(false),
     myPen(Qt::black),
     myThickness(DEFAULT_BOND_THICKNESS),
     dashedLine(false),
     myLabel(0)
 {
     setFlag(QGraphicsItem::ItemIsSelectable, true);
	 setAcceptsHoverEvents(true);
	 updatePosition();
	 myPen.setWidthF(hoverOver ? 1.5*effectiveWidth : effectiveWidth); 	
     myLength = computeLength(); 
 }


 void Bond::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
 {
	// A null event to prevent unwanted deselection
 }


 void Bond::mousePressEvent(QGraphicsSceneMouseEvent *event)
 {
	// A null event to prevent unwanted deselection
 }
 
 
 void Bond::toggleLabel()
 {
	 if(myLabel == 0){
		 myLabel = new Label(Label::BondLabelType, myLength, DEFAULT_BOND_LABEL_PRECISION, drawingInfo);
//		 connect(myLabel, SIGNAL(lostFocus(Label*)), mainWindow, SLOT(editorLostFocus(Label*)));
		 // Time to figure out how to make the bond labels appear correctly
		 updatePosition();
	 }else{
		 delete myLabel;
		 myLabel = 0;
	 }
 }
 
 
 void Bond::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
 {
	 Q_UNUSED(event);
	 hoverOver = true;
	 update();
 }

 
 void Bond::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
 {
	 Q_UNUSED(event);
	 hoverOver = false;
	 update();
 }

 
 void Bond::updatePosition()
 {
 	Atom *atom1 = myStartAtom; 
	Atom *atom2 = myEndAtom;
	
	double radius1 		= atom1->effectiveRadius();
	double radius2 		= atom2->effectiveRadius();
	double x1     		= atom1->pos().x();
	double y1     		= atom1->pos().y();
	double z1     		= atom1->zValue();
	double x2     		= atom2->pos().x();
	double y2     		= atom2->pos().y();
	double z2     		= atom2->zValue();
	double dx     		= x2-x1;
	double dy     		= y2-y1;
	double dz     		= z2-z1;
	double norm   		= sqrt(dx*dx + dy*dy);
	// Phi is the angle subtended by the bond and the y axis(which runs negative)
	double sinPhi    	= (norm == 0.0 ? 0.0 : dx/norm);
	double cosPhi       = (norm == 0.0 ? 0.0 : dy/norm);
	// The angle subtended by the bond and the z axis
	double r      		= sqrt(dx*dx + dy*dy + dz*dz);
	double sinTheta 	= norm/r;
	QPointF startPoint;
	QPointF endPoint;
	
	startPoint.setX(x1 + ( radius1*sinTheta*sinPhi) );
	startPoint.setY(y1 + ( radius1*sinTheta*cosPhi) );
	endPoint.setX(  x2 - ( radius2*sinTheta*sinPhi) );
	endPoint.setY(  y2 - ( radius2*sinTheta*cosPhi) );
	setLine(QLineF(startPoint, endPoint));

	// The average position is used to determine the scale factor
	// the TINY is there in case both atoms are at the same z
	// in which case we must guarantee the bond goes behind the atoms
	setZValue((atom1->zValue() + atom2->zValue())/2.0 - TINY);
 	myPen.setCapStyle( (fabs(dz) > DZ_ZERO_TOL ? Qt::RoundCap : Qt::SquareCap));
 	effectiveWidth = (1.0 + drawingInfo->perspective() * zValue()) * myThickness * drawingInfo->scaleFactor();
 
 	// A cheap and cheerful guess of the label's position
	if(myLabel != 0){
	 	double length = drawingInfo->scaleFactor() * myLength;
	 	// Account for differing radii	 	
	 	double rMidPoint = radius1 + (length - radius1 - radius2)/2.0;
	 	QPointF labelPos;
	    labelPos.setX(myLabel->dX() + x1 + rMidPoint*sinTheta*sinPhi);
	    labelPos.setY(myLabel->dY() + y1 + rMidPoint*sinTheta*cosPhi);    
	    myLabel->setPos(labelPos);
	}
 }
 
 
 void Bond::toggleDashing()
 {
	 if(dashedLine){
		 myThickness = DEFAULT_BOND_THICKNESS;
		 myPen.setStyle(Qt::SolidLine);
		 dashedLine = false;
	 }else{
		 myThickness = DEFAULT_DASHED_BOND_THICKNESS;
		 myPen.setStyle(Qt::CustomDashLine);
		 QVector<qreal> dashes;
		 qreal space = 3;
		 qreal dash  = 2;
		 dashes << dash << space;
		 myPen.setDashPattern(dashes);
		 dashedLine = true;
	 }
 }
 
 
 void Bond::paint(QPainter *painter,
            const QStyleOptionGraphicsItem *option, QWidget *widget)
 {
	myPen.setWidthF(hoverOver ? 1.5*effectiveWidth : effectiveWidth);
 	myPen.setColor(myColor);
	painter->setBrush(myColor);
	painter->setPen(myPen);
 	painter->drawLine(line());
 	if(isSelected()){
	 	myPen.setColor(SELECTED_COLOR);
		painter->setPen(myPen);
        painter->drawLine(line());        	
    }
 }

 double Bond::computeLength(){
	 return(sqrt(	pow(myStartAtom->x()-myEndAtom->x(),2.0)+
	 		  		pow(myStartAtom->y()-myEndAtom->y(),2.0)+
	 		  		pow(myStartAtom->z()-myEndAtom->z(),2.0)));	 
 }
 