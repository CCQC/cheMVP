#include <QtGui>

 #include "label.h"
 #include "drawingcanvas.h"

 Label::Label(LabelType type, double value, int precision, DrawingInfo *info, QGraphicsItem *parent, QGraphicsScene *scene)
     : QGraphicsTextItem(parent, scene),
     myType(type),
     myValue(value),
     myPrecision(precision),
     myDrawingInfo(info),
     myColor(Qt::black),
     myDY(0.0),
     myDX(0.0)
 {
     setFlag(QGraphicsItem::ItemIsMovable, true);
     setFlag(QGraphicsItem::ItemIsSelectable, true);
     setTextInteractionFlags(Qt::NoTextInteraction);
     setZValue(1000.0);
     updateLabel();
     setFont(DEFAULT_LABEL_FONT);
     setFontSize(DEFAULT_LABEL_FONT_SIZE);

 }

 
// QVariant Label::itemChange(GraphicsItemChange change,
//                      const QVariant &value)
// {
//     if (change == QGraphicsItem::ItemSelectedChange)
//         emit selectedChange(this);
//     return value;
// }

 
// void Label::paint(QPainter *painter,
//            const QStyleOptionGraphicsItem *option, QWidget *widget)
// {
//	 painter->setPen(myColor);
//	 painter->setFont(myFont);
//     painter->drawText(pos(), myString); 
// }
	 
 void Label::updateLabel()
 {
	 myString.setNum(myValue, 'f', myPrecision);
	 setPlainText(myString);
	 QGraphicsTextItem::setFont(myFont);
 }
 
 
 void Label::focusOutEvent(QFocusEvent *event)
 {
     setTextInteractionFlags(Qt::NoTextInteraction);
//     emit lostFocus(this);
     QGraphicsTextItem::focusOutEvent(event);
 }

 
// void Label::mousePressEvent(QGraphicsSceneMouseEvent *event)
// {
// }


// void Label::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
// {
// }

 
// void Label::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
// {
//     if (textInteractionFlags() & Qt::TextEditable) {
//    	 QGraphicsTextItem::mouseMoveEvent(event);
//     }else{
//    	 QGraphicsItem::mouseMoveEvent(event);
//     }
// }

 
 void Label::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
 {
	 if (textInteractionFlags() == Qt::NoTextInteraction)
	          setTextInteractionFlags(Qt::TextEditorInteraction);
	      QGraphicsTextItem::mouseDoubleClickEvent(event);
 }
