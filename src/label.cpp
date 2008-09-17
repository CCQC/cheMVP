#include <QtGui>
#include <QtDebug>

 #include "label.h"
 #include "drawingcanvas.h"

 Label::Label(LabelType type, double value, int precision, DrawingInfo *info, QGraphicsItem *parent, QGraphicsScene *scene)
     : QGraphicsTextItem(parent, scene),
     myType(type),
     myValue(value),
     myPrecision(precision),
     myDrawingInfo(info),
     myColor(Qt::black),
     myFontSize(DEFAULT_LABEL_FONT_SIZE),
     myDY(0.0),
     myDX(0.0)
 {
     setFlag(QGraphicsItem::ItemIsMovable);
     setFlag(QGraphicsItem::ItemIsSelectable);
     setFlag(QGraphicsItem::ItemIsFocusable);
	 setTextInteractionFlags(Qt::NoTextInteraction);    	 
     setZValue(1000.0);
     if(myType != TextLabelType){
    	 updateLabel();
     }
     setFont(QFont(DEFAULT_LABEL_FONT));
     updateFontSize();
     // If the zoom factor changes or the window size changes, we need to adjust
     // the size of the font for the labels accordingly
	 connect(myDrawingInfo, SIGNAL(scaleFactorChanged()), this, SLOT(updateFontSize()));
	 setToolTip(tr("Double click to edit"));
 }

 
 void Label::focusOutEvent(QFocusEvent *event)
 {
	 qDebug() <<"focus out";
	 if(event->reason()==Qt::TabFocusReason && textInteractionFlags() == Qt::TextEditorInteraction){
  		 textCursor().insertText("\t");
//  		 QFocusEvent *focusIn = new QFocusEvent(QEvent::FocusIn);
//  		 QGraphicsTextItem::focusInEvent(focusIn);
  		 setTextInteractionFlags(Qt::TextEditorInteraction);
	 }else{
  		 setTextInteractionFlags(Qt::NoTextInteraction);
		 QGraphicsTextItem::focusOutEvent(event);
	 }
 }

  
 void Label::updateFontSize()
 {
	 QFont myFont(font());
 	 myFont.setPointSizeF(double(myFontSize)*myDrawingInfo->scaleFactor()/100.0);
 	 setFont(myFont);
 }
 
 
 void Label::updateLabel()
 {
	 myString.setNum(myValue, 'f', myPrecision);
	 setPlainText(myString);
 }

 
 void Label::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
 {
	 qDebug() <<"double click";
	 if (textInteractionFlags() == Qt::NoTextInteraction){
	   setTextInteractionFlags(Qt::TextEditorInteraction);
	 }
	 QGraphicsTextItem::mouseDoubleClickEvent(event);
 }
